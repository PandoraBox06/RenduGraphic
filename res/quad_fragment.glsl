#version 410

out vec4 out_color;
in vec2 uv;

uniform sampler2D screen_texture;

void main()
{
    // Sample the original texture
    vec4 color = texture(screen_texture, uv);

    // Convert to grayscale using luminance formula
    float grayscale = dot(color.rgb, vec3(0.299, 0.587, 0.114));

    // Set final color as grayscale
    out_color = vec4(vec3(grayscale), color.a);
}