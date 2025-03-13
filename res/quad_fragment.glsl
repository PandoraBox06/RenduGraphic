#version 410

out vec4 out_color;

in vec2 frag_uv;
uniform sampler2D screen_texture;

void main()
{
    vec4 texture = texture(screen_texture, frag_uv);
    out_color = vec4(texture.r, texture.g, texture.b, 1.0);
}