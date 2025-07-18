#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>

glm::vec2 P0 = {-0.8f, -0.6f}; // Point fixe gauche
glm::vec2 P2 = { 0.8f,  0.6f}; // Point fixe droite

glm::vec2 get_mouse_ndc()
{
    glm::vec2 mouse = gl::mouse_position();
    float aspect = gl::framebuffer_aspect_ratio();
    return {
        (mouse.x - 0.5f) * 2.f * aspect,
        -(mouse.y - 0.5f) * 2.f
    };
}

// De Casteljau quadratique
glm::vec2 de_casteljau(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t)
{
    glm::vec2 a = (1 - t) * p0 + t * p1;
    glm::vec2 b = (1 - t) * p1 + t * p2;
    return (1 - t) * a + t * b;
}

std::vector<glm::vec2> compute_bezier_curve(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, int resolution = 64)
{
    std::vector<glm::vec2> curve;
    for (int i = 0; i <= resolution; ++i)
    {
        float t = (float)i / resolution;
        curve.push_back(de_casteljau(p0, p1, p2, t));
    }
    return curve;
}

int main()
{
    gl::init("Casteljau Quadratique - Point souris");
    gl::maximize_window();

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 P1 = get_mouse_ndc(); // point de contrôle mobile

        // Calcul et dessin de la courbe
        auto curve = compute_bezier_curve(P0, P1, P2, 100);
        utils::draw_polyline(curve, false, 0.005f, {1, 1, 1, 1}); // courbe blanche

        // Points de contrôle
        utils::draw_disk(P0, 0.01f, {1, 0, 1, 1}); // violet
        utils::draw_disk(P1, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P2, 0.01f, {1, 0, 1, 1});

        // Lignes de contrôle
        utils::draw_line(P0, P1, 0.002f, {1, 1, 1, 0.2f});
        utils::draw_line(P1, P2, 0.002f, {1, 1, 1, 0.2f});
    }
}
