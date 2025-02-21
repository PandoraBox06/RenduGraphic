#version 410

out vec4 out_color;
in vec2 uv;

uniform sampler2D screen_texture;

void main()
{
       // Sample the original color from the render texture
    vec4 color = texture(screen_texture, uv);

    // Convert to grayscale using luminance formula
    float grayscale = dot(color.rgb, vec3(0.299, 0.587, 0.114));

    // Keep red color, convert others to grayscale
    vec3 final_color = vec3(color.r, grayscale, grayscale);

    // Output final color
    out_color = vec4(final_color, color.a);
}