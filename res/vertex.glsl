#version 410

layout(location = 0) in vec3 in_position;
uniform float aspect_ratio;
uniform float time;
uniform mat4 view_projection_matrix;
uniform mat4 model_matrix;
out vec3 vertex_position;
void main()
{
    vertex_position = in_position;
    gl_Position = view_projection_matrix * model_matrix * vec4(in_position, 1.);
}