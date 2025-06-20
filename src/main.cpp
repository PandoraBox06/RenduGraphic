#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <optional>

float cross(glm::vec2 a, glm::vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

/**
 * Calculates the intersection point of two line segments AB and CD.
 * Mathematical approach:
 * 1. Parametric form of segments:
 *    - AB = A + t*r where r = B-A and t ∈ [0,1]
 *    - CD = C + u*s where s = D-C and u ∈ [0,1]
 *
 * 2. At intersection: A + t*r = C + u*s
 *    Cross product with s: (A + t*r - C) × s = 0
 *    t = ((C-A) × s)/(r × s)
 *
 * 3. Similarly for u:
 *    u = ((C-A) × r)/(r × s)
 *
 * 4. Intersection exists if t,u ∈ [0,1] and (r × s) ≠ 0
 */

std::optional<glm::vec2> intersect_segments(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D)
{
    // Direction vectors of segments
    glm::vec2 r = B - A;
    glm::vec2 s = D - C;
    glm::vec2 diff = C - A;

    // Cross products for intersection calculation
    float rxs = cross(r, s);
    float expr = cross(diff, r);

    // Check if segments are parallel or colinear
    if (rxs == 0.f)
        return std::nullopt;

    // Calculate intersection parameters t and u
    float t = cross(diff, s) / rxs;
    float u = cross(diff, r) / rxs;

    // Check if intersection point lies within both segments
    if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f)
        return A + t * r;

    return std::nullopt;
}

glm::vec2 reflect(glm::vec2 velocity, glm::vec2 normal)
{
    return velocity - 2.f * glm::dot(velocity, normal) * normal;
}

struct Particle
{
    glm::vec2 position{
        utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
        utils::rand(-1.f, +1.f),
    };
    glm::vec2 velocity;

    glm::vec3 color{
        utils::rand(0.f, 1.f),
        utils::rand(0.f, 1.f),
        utils::rand(0.f, 1.f),
    };

    Particle()
    {
        float angle = utils::rand(0.f, 2.f * glm::pi<float>());
        velocity = 0.3f * glm::vec2(std::cos(angle), std::sin(angle));
    }

    float radius() const
    {
        return 0.015f;
    }
};

struct Segment
{
    glm::vec2 A;
    glm::vec2 B;

    glm::vec2 normal() const
    {
        glm::vec2 dir = B - A;
        glm::vec2 n = glm::normalize(glm::vec2(-dir.y, dir.x)); // Perpendicular
        return n;
    }
};

int main()
{
    gl::init("Particles Bounce on Segments");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i)
        particles.emplace_back();

    std::vector<Segment> segments;
    for (int i = 0; i < 5; ++i)
    {
        glm::vec2 A{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.f, +1.f),
        };
        glm::vec2 B{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.f, +1.f),
        };
        segments.push_back({A, B});
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw segments
        for (auto const &seg : segments)
            utils::draw_line(seg.A, seg.B, 0.008f, {0.f, 1.f, 1.f, 1.f}); // Cyan

        for (auto &particle : particles)
        {
            glm::vec2 previous = particle.position;

            // Apply air friction
            glm::vec2 force = -particle.velocity * gl::delta_time_in_seconds();
            particle.velocity += force * gl::delta_time_in_seconds();

            glm::vec2 new_pos = particle.position + particle.velocity * gl::delta_time_in_seconds();

            for (auto const &seg : segments)
            {
                if (auto hit = intersect_segments(previous, new_pos, seg.A, seg.B))
                {
                    // Reflect
                    glm::vec2 n = seg.normal();
                    particle.velocity = reflect(particle.velocity, -n);
                    new_pos = *hit + particle.velocity * gl::delta_time_in_seconds();
                }
            }

            particle.position = new_pos;

            utils::draw_disk(particle.position, particle.radius(), glm::vec4(particle.color, 1.f));
        }
    }
}