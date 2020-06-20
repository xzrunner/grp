#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

//////////////////////////////////////////////////////////////////////////
// resource
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Resource

#define PARM_NODE_TYPE RenderTarget
#define PARM_NODE_NAME render_target
#include EXE_FILEPATH

#define PARM_NODE_TYPE Texture
#define PARM_NODE_NAME texture
#define ENABLE_NODE_PREVIEW
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE TextureCube
#define PARM_NODE_NAME texture_cube
#include EXE_FILEPATH

#define PARM_NODE_TYPE PrimitiveShape
#define PARM_NODE_NAME primitive_shape
#include EXE_FILEPATH

#define PARM_NODE_TYPE Heightfield
#define PARM_NODE_NAME heightfield
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// operator
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Operator

#define PARM_NODE_TYPE Clear
#define PARM_NODE_NAME clear
//#define ENABLE_NODE_PREVIEW
#include EXE_FILEPATH

#define PARM_NODE_TYPE Bind
#define PARM_NODE_NAME bind
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Unbind
#define PARM_NODE_NAME unbind
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Draw
#define PARM_NODE_NAME draw
#include EXE_FILEPATH

#define PARM_NODE_TYPE SetUniform
#define PARM_NODE_NAME set_uniform
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// state
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME State

#define PARM_NODE_TYPE Viewport
#define PARM_NODE_NAME viewport
#include EXE_FILEPATH

#define PARM_NODE_TYPE AlphaTest
#define PARM_NODE_NAME alpha_test
#include EXE_FILEPATH

#define PARM_NODE_TYPE BlendEq
#define PARM_NODE_NAME blend_eq
#include EXE_FILEPATH

#define PARM_NODE_TYPE BlendFunc
#define PARM_NODE_NAME blend_func
#include EXE_FILEPATH

#define PARM_NODE_TYPE Cull
#define PARM_NODE_NAME cull
#include EXE_FILEPATH

#define PARM_NODE_TYPE ZTest
#define PARM_NODE_NAME ztest
#include EXE_FILEPATH

#define PARM_NODE_TYPE ZWrite
#define PARM_NODE_NAME zwrite
#include EXE_FILEPATH

#define PARM_NODE_TYPE FrontFace
#define PARM_NODE_NAME front_face
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// value
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Value

#define PARM_NODE_TYPE Int
#define PARM_NODE_NAME int
#include EXE_FILEPATH

#define PARM_NODE_TYPE Bool
#define PARM_NODE_NAME bool
#include EXE_FILEPATH

#define PARM_NODE_TYPE Vector1
#define PARM_NODE_NAME vector1
#include EXE_FILEPATH

#define PARM_NODE_TYPE Vector2
#define PARM_NODE_NAME vector2
#include EXE_FILEPATH

#define PARM_NODE_TYPE Vector3
#define PARM_NODE_NAME vector3
#include EXE_FILEPATH

#define PARM_NODE_TYPE Vector4
#define PARM_NODE_NAME vector4
#include EXE_FILEPATH

#define PARM_NODE_TYPE Matrix2
#define PARM_NODE_NAME matrix2
#include EXE_FILEPATH

#define PARM_NODE_TYPE Matrix3
#define PARM_NODE_NAME matrix3
#include EXE_FILEPATH

#define PARM_NODE_TYPE Matrix4
#define PARM_NODE_NAME matrix4
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// math
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Math

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Add
#define PARM_NODE_NAME add
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Mul
#define PARM_NODE_NAME mul
#include EXE_FILEPATH

#define PARM_NODE_TYPE PerspectiveMat
#define PARM_NODE_NAME perspective_mat
#include EXE_FILEPATH

#define PARM_NODE_TYPE OrthoMat
#define PARM_NODE_NAME ortho_mat
#include EXE_FILEPATH

#define PARM_NODE_TYPE LookAtMat
#define PARM_NODE_NAME look_at_mat
#include EXE_FILEPATH

#define PARM_NODE_TYPE Translate
#define PARM_NODE_NAME translate
#include EXE_FILEPATH

#define PARM_NODE_TYPE Rotate
#define PARM_NODE_NAME rotate
#include EXE_FILEPATH

#define PARM_NODE_TYPE Scale
#define PARM_NODE_NAME scale
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// input
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME Input

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE ProjectMat
#define PARM_NODE_NAME project_mat
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE ViewMat
#define PARM_NODE_NAME view_mat
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE ModelMat
#define PARM_NODE_NAME model_mat
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE CameraPosition
#define PARM_NODE_NAME camera_position
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE LightPosition
#define PARM_NODE_NAME light_position
#include EXE_FILEPATH

#define PARM_NODE_TYPE UserScript
#define PARM_NODE_NAME user_script
#include EXE_FILEPATH

#undef NODES_GROUP_NAME

//////////////////////////////////////////////////////////////////////////
// control flow
//////////////////////////////////////////////////////////////////////////

#define NODES_GROUP_NAME ControlFlow

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE ForEachLoop
#define PARM_NODE_NAME for_each_loop
#include EXE_FILEPATH

#undef NODES_GROUP_NAME
