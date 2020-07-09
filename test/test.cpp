#include <sx/ResFileHelper.h>
#include <ns/NodeFactory.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <unirender/Factory.h>
#include <ns/RegistCallback.h>
#include <painting3/PerspCam.h>
#include <facade/Facade.h>
#include <renderpipeline/RenderMgr.h>
#include <rendergraph/RenderGraph.h>
#include <rendergraph/RenderContext.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/ScriptEnv.h>
#include <js/RapidJsonHelper.h>
#include <dag/Node.h>
#include <unirender/typedef.h>
#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/TextureDescription.h>
#include <unirender/Framebuffer.h>
#include <unirender/Context.h>
#include <unirender/DrawState.h>
#include <unirender/ClearState.h>
#include <gimg_export.h>
#include <gimg_typedef.h>

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/Blueprint.h>
#include <blueprint/CompNode.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/SerializeHelper.h>
#include <renderlab/Node.h>
#include <renderlab/RenderLab.h>
#include <renderlab/RenderAdapter.h>
#include <renderlab/RegistNodes.h>
#include <renderlab/Serializer.h>
#include <renderlab/node/PassEnd.h>

#include <gl/glew.h>
#include <glfw3.h>

#include <boost/filesystem.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace
{

const char* vs = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)";

const int TEX_SIZE = 512;

uint8_t* BUFFER = nullptr;

}

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

bool init_gl()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(100, 100, "rotate-crop", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	//// Initialize GLEW to setup the OpenGL Function pointers
	//if (glewInit() != GLEW_OK) {
	//	return -1;
	//}

	return true;
}

void build_drawlist(const std::vector<bp::NodePtr>& f_nodes, const std::vector<std::shared_ptr<dag::Node<rendergraph::Variable>>>& b_nodes,
                    std::vector<std::unique_ptr<rendergraph::DrawList>>& initlist, std::vector<std::unique_ptr<rendergraph::DrawList>>& drawlist)
{
    // sort passes
    std::vector<std::shared_ptr<renderlab::node::PassEnd>> passes;
    for (auto& node : f_nodes) {
        if (node->get_type() == rttr::type::get<renderlab::node::PassEnd>()) {
            passes.push_back(std::static_pointer_cast<renderlab::node::PassEnd>(node));
        }
    }
    std::sort(passes.begin(), passes.end(), [](
        const std::shared_ptr<renderlab::node::PassEnd>& p0,
        const std::shared_ptr<renderlab::node::PassEnd>& p1) -> bool {
        return p0->GetOrder() < p1->GetOrder();
    });

    // gen drawlist
    for (auto& pass : passes)
    {
        auto& conns = pass->GetAllInput()[0]->GetConnecting();
        if (conns.empty()) {
            continue;
        }
        auto& bp_node = conns[0]->GetFrom()->GetParent();

        int idx = -1;
        for (int i = 0, n = f_nodes.size(); i < n; ++i) {
            if (f_nodes[i].get() == &bp_node) {
                idx = i;
                break;
            }
        }
        assert(idx >= 0);
        auto back_node = b_nodes[idx];
        auto rg_node = std::static_pointer_cast<rendergraph::Node>(back_node);
        std::vector<rendergraph::NodePtr> init_nodes, draw_nodes;
        rendergraph::DrawList::GetAntecedentNodes(rg_node, init_nodes, false);
        rendergraph::DrawList::GetAntecedentNodes(rg_node, draw_nodes);
        initlist.push_back(std::make_unique<rendergraph::DrawList>(init_nodes));
        drawlist.push_back(std::make_unique<rendergraph::DrawList>(draw_nodes));
    }
}

void draw(std::shared_ptr<rendergraph::ScriptEnv>& script, const ur::Device& dev,
          ur::Context& ctx, const pt0::Camera& cam, const std::vector<std::unique_ptr<rendergraph::DrawList>>& drawlist)
{
    rp::RenderMgr::Instance()->SetRenderer(dev, ctx, rp::RenderType::EXTERN);

    auto rc = std::make_shared<rendergraph::RenderContext>(script);
    script->SetRenderContext(rc);
    rc->cam_proj_mat = cam.GetProjectionMat();
    rc->cam_view_mat = cam.GetViewMat();
    if (cam.TypeID() == pt0::GetCamTypeID<pt3::PerspCam>()) {
        auto& p_cam = static_cast<const pt3::PerspCam&>(cam);
        rc->cam_position = p_cam.GetPos();
    }
    rc->light_position.Set(0, 2, -4);

    rc->ur_dev = &dev;
    rc->ur_ctx = &ctx;

    rc->screen_size.Set(TEX_SIZE, TEX_SIZE);

    for (auto& p : drawlist) {
        bool finished = p->Draw(rc, nullptr);
        if (finished) {
            break;
        }
    }
}

void test_file(const ur::Device& dev, ur::Context& ctx,
               const std::shared_ptr<ur::Framebuffer>& rt,
               const std::string& filepath)
{
    auto dir = boost::filesystem::path(filepath).parent_path().string();

    auto root = ns::NodeFactory::Create(dev, filepath);

    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    auto& children = ccomplex.GetAllChildren();

    std::vector<bp::NodePtr> front_nodes(children.size(), nullptr);
    std::vector<std::shared_ptr<dag::Node<rendergraph::Variable>>> back_nodes(children.size(), nullptr);

    bp::SerializeHelper::SetupNodes<renderlab::Node, rendergraph::Variable>(
        children, "renderlab", "rendergraph", front_nodes, back_nodes
    );
    assert(front_nodes.size() == back_nodes.size());
    for (int i = 0, n = front_nodes.size(); i < n; ++i) {
        renderlab::RenderAdapter::Front2Back(dev, *front_nodes[i], *back_nodes[i], dir);
    }
    bp::SerializeHelper::SetupConnections(filepath, children, front_nodes, back_nodes);

    ctx.SetFramebuffer(rt);
    ctx.SetViewport(0, 0, TEX_SIZE, TEX_SIZE);

    ur::ClearState clear;
    clear.buffers = ur::ClearBuffers::ColorAndDepthBuffer;
    clear.color.FromRGBA(0x88888888);
    ctx.Clear(clear);

    auto script = std::make_shared<rendergraph::ScriptEnv>();

	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
	pt0::CameraPtr cam = std::make_shared<pt3::PerspCam>(
		sm::vec3(0, 2, -2), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0)
	);
	renderlab::Serializer::LoadCamera(cam, doc);

    std::vector<std::unique_ptr<rendergraph::DrawList>> initlist, drawlist;
    build_drawlist(front_nodes, back_nodes, initlist, drawlist);
    draw(script, dev, ctx, *cam, initlist);
    draw(script, dev, ctx, *cam, drawlist);

    dev.ReadPixels(BUFFER, ur::TextureFormat::RGB, 0, 0, TEX_SIZE, TEX_SIZE);

    auto out_filepath = filepath.substr(0, filepath.find_last_of('.')) + ".jpg";
    gimg_export(out_filepath.c_str(), BUFFER, TEX_SIZE, TEX_SIZE, GPF_RGB, false);

    ctx.SetFramebuffer(nullptr);
}

void test_folder(const ur::Device& dev, ur::Context& ctx,
                 const std::string& dir)
{
    auto fbo = dev.CreateFramebuffer();

	auto color_rbo = dev.CreateRenderBuffer(TEX_SIZE, TEX_SIZE, ur::InternalFormat::RGB, ur::AttachmentType::Color0);
    fbo->SetAttachment(ur::AttachmentType::Color0, ur::TextureTarget::Texture2D, nullptr, color_rbo);

	auto depth_rbo = dev.CreateRenderBuffer(TEX_SIZE, TEX_SIZE, ur::InternalFormat::DepthComponent, ur::AttachmentType::Depth);
	fbo->SetAttachment(ur::AttachmentType::Depth, ur::TextureTarget::Texture2D, nullptr, depth_rbo);

    boost::filesystem::recursive_directory_iterator itr(dir), end;
    while (itr != end)
    {
        if (boost::filesystem::is_regular(itr->path()))
        {
            auto path = itr->path().string();
            if (sx::ResFileHelper::Type(path) == sx::ResFileType::RES_FILE_JSON) {
                test_file(dev, ctx, fbo, path);
            }
        }
        ++itr;
    }
}

int main()
{
    init_gl();

    auto dev = ur::CreateDevice(ur::APIType::OpenGL, nullptr);
    auto ctx = ur::CreateContext(ur::APIType::OpenGL , *dev);

    ns::RegistCallback::Init();
    facade::Facade::Instance()->Init(*dev);
    bp::Blueprint::Instance();
    rendergraph::RenderGraph::Instance();
    renderlab::RenderLab::Instance();

    BUFFER = new uint8_t[TEX_SIZE * TEX_SIZE * 4];

    // fixme: id
    auto node = std::make_shared<n0::SceneNode>();
    node->AddSharedComp<n0::CompComplex>();

    test_folder(*dev, *ctx, "../../../test/cases");

    return 0;
}