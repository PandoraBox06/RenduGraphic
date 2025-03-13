#version 410

uniform sampler2D my_texture;
in vec2 uv;
out vec4 out_color;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    out_color = vec4(texture_color);
}