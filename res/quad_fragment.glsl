#version 410

out vec4 out_color;
in vec2 uv;

uniform sampler2D screen_texture;

void main()
{
       // Sample the original color from the render texture
    vec4 color = texture(screen_texture, uv);

    // Output final color
    out_color = vec4(color.r, 0, 0, color.a);
}