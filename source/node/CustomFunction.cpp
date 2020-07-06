#include "renderlab/node/CustomFunction.h"
#include "renderlab/RenderAdapter.h"

#include <blueprint/Pin.h>

#include <rendergraph/node/FunctionParser.h>

namespace renderlab
{
namespace node
{

void CustomFunction::SetCode(const std::string& code)
{
    if (m_code == code) {
        return;
    }

    m_code = code;

    rendergraph::node::FunctionParser parser(code);
    parser.Parse();

    if (IsPinsChanged(true, parser.GetInputs()))
    {
        m_all_input.erase(m_all_input.begin() + 1, m_all_input.end());
        for (auto& i : parser.GetInputs()) {
            auto type = RenderAdapter::TypeBackToFront(i.type, i.count);
            auto pin = std::make_shared<bp::Pin>(true, m_all_input.size(), type, i.name, *this);
            m_all_input.push_back(pin);
        }
    }

    if (IsPinsChanged(false, parser.GetOutputs()))
    {
        m_all_output.erase(m_all_output.begin() + 1, m_all_output.end());
        for (auto& o : parser.GetOutputs()) {
            auto type = RenderAdapter::TypeBackToFront(o.type, o.count);
            auto pin = std::make_shared<bp::Pin>(false, m_all_output.size(), type, o.name, *this);
            m_all_output.push_back(pin);
        }
    }

    m_title = parser.GetName();

    Layout();

    SetSizeChanged(true);
}

bool CustomFunction::IsPinsChanged(bool is_in, const std::vector<rendergraph::Variable>& new_vars) const
{
    auto& old_pins = is_in ? m_all_input : m_all_output;
    if (old_pins.size() != new_vars.size() + 1) {
        return true;
    }

    if (new_vars.size() == 0) {
        return false;
    }

    assert(old_pins.size() > 0);
    for (int i = 0, n = old_pins.size() - 1; i < n; ++i)
    {
        auto type = RenderAdapter::TypeBackToFront(new_vars[i].type, new_vars[i].count);
        if (type != old_pins[1 + i]->GetOldType() ||
            new_vars[i].name != old_pins[1 + i]->GetName()) {
            return true;
        }
    }

    return false;
}

}
}