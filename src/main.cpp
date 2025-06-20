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

int main()
{
    gl::init("Segment-Segment Intersection");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Fixed segment in center
        glm::vec2 A = {-0.5f, 0.f};
        glm::vec2 B = {0.5f, 0.f};

        // Mouse-based segment
        glm::vec2 mouse = gl::mouse_position();
        glm::vec2 C = mouse - glm::vec2(0.3f, 0.2f);
        glm::vec2 D = mouse + glm::vec2(0.3f, 0.2f);

        // Draw segments
        utils::draw_line(A, B, 0.01f, {1.f, 1.f, 1.f, 1.f}); // white
        utils::draw_line(C, D, 0.01f, {0.f, 1.f, 1.f, 1.f}); // cyan

        // Draw intersection if it exists
        if (auto point = intersect_segments(A, B, C, D))
        {
            utils::draw_disk(*point, 0.02f, {1.f, 1.f, 0.f, 1.f}); // yellow
        }
    }
}
