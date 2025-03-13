#version 410

out vec4 out_color;
in vec3 position;
in vec2 uv;
in vec3 normal;
uniform sampler2D my_texture;
uniform vec3 light_direction;
uniform vec3 point_light;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    float dir_light_intensity = dot(normalize(normal), light_direction);
    vec3 dir_to_point_light = normalize(position - point_light);
    float point_light_intensity = dot(normalize(normal), dir_to_point_light);
    // produit_scalaire += 0.3;
    out_color = vec4((dir_light_intensity + point_light_intensity + 0.3) * texture_color);
    // out_color.rgb = position;
}