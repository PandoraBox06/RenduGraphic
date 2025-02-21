#version 410

out vec4 out_color;
in vec2 uv;

uniform sampler2D screen_texture;

void main()
{
    out_color = texture(screen_texture, uv);
}
