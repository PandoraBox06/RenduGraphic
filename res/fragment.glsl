#version 410

in vec3 position_ws;
in vec2 uv;
in vec3 normal_ws;
in vec4 frag_position_light_space;

uniform sampler2D texture_sampler;
uniform sampler2D shadow_map;

uniform vec3 light_color;
uniform vec3 light_position_ws;

out vec4 out_color;

float calculate_shadow(vec4 frag_pos_light)
{
    vec3 proj_coords = frag_pos_light.xyz / frag_pos_light.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;

    float bias = 0.005;
    float shadow = current_depth - bias > closest_depth ? 0.5 : 1.0;

    return shadow;
}

void main()
{
    vec3 albedo = texture(texture_sampler, uv).rgb;

    vec3 light_dir = normalize(light_position_ws - position_ws);
    float diff = max(dot(normal_ws, light_dir), 0.0);

    float shadow = calculate_shadow(frag_position_light_space);

    vec3 color = albedo * light_color * diff * shadow;

    out_color = vec4(color, 1.0);
}