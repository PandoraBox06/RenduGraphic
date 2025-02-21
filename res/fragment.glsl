#version 410

out vec4 out_color;

in vec2 uv;              // UV coordinates received from vertex shader
in vec3 vertex_position; // Position from vertex shader

uniform sampler2D my_texture; // Texture sampler

void main()
{
    // Sample texture using UV coordinates
    vec4 texture_color = texture(my_texture, uv);
    // If no texture, fallback to a debug color using UV mapping
    out_color = texture_color;
}
