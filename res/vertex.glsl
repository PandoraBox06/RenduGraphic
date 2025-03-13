#version 410

layout(location = 0) in vec3 in_position_os;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal_os;

uniform mat4 model_view_projection_matrix;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;
uniform mat4 light_space_matrix;

out vec3 position_ws;
out vec2 uv;
out vec3 normal_ws;
out vec4 frag_position_light_space;

void main()
{
    position_ws = (model_matrix * vec4(in_position_os, 1.0)).xyz;

    normal_ws = normalize((normal_matrix * vec4(in_normal_os, 0.0)).xyz);

    uv = in_uv;

    frag_position_light_space = light_space_matrix * vec4(position_ws, 1.0);

    gl_Position = model_view_projection_matrix * vec4(in_position_os, 1.0);
}