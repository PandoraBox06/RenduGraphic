#version 410

layout(location = 0) in vec3 in_position; // Vertex position
layout(location = 1) in vec2 in_uv;       // UV coordinates

uniform float aspect_ratio;
uniform float time;
uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;

out vec2 uv; // Pass UV to fragment shader
out vec3 vertex_position;

void main()
{
    vertex_position = in_position;
    uv = in_uv; // Pass the UV coordinates to fragment shader
    gl_Position = view_projection_matrix * model_matrix * vec4(in_position, 1.0);
}
