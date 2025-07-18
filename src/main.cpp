#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>

// Points de contrôle fixes
glm::vec2 P0 = {-0.8f, -0.6f};
glm::vec2 P1 = {-0.4f,  0.8f};
glm::vec2 P2 = { 0.4f,  0.8f};
glm::vec2 P3 = { 0.8f, -0.6f};

// De Casteljau cubique
glm::vec2 de_casteljau(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
    glm::vec2 a = (1 - t) * p0 + t * p1;
    glm::vec2 b = (1 - t) * p1 + t * p2;
    glm::vec2 c = (1 - t) * p2 + t * p3;

    glm::vec2 d = (1 - t) * a + t * b;
    glm::vec2 e = (1 - t) * b + t * c;

    return (1 - t) * d + t * e;
}

// Courbe complète
std::vector<glm::vec2> compute_bezier_curve(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, int resolution = 64)
{
    std::vector<glm::vec2> curve;
    for (int i = 0; i <= resolution; ++i)
    {
        float t = static_cast<float>(i) / resolution;
        curve.push_back(de_casteljau(p0, p1, p2, p3, t));
    }
    return curve;
}

// Particules régulièrement espacées
void spawn_particles_on_curve_regular(
    std::vector<glm::vec2>& out_particles,
    glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
    int count)
{
    for (int i = 0; i < count; ++i)
    {
        float t = static_cast<float>(i) / (count - 1); // [0, 1] inclus
        glm::vec2 pt = de_casteljau(p0, p1, p2, p3, t);
        out_particles.push_back(pt);
    }
}

int main()
{
    gl::init("Particules espacées régulièrement");
    gl::maximize_window();

    std::vector<glm::vec2> particles;
    std::vector<glm::vec2> curve = compute_bezier_curve(P0, P1, P2, P3, 100);
    spawn_particles_on_curve_regular(particles, P0, P1, P2, P3, 100);

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Courbe
        utils::draw_polyline(curve, false, 0.005f, {1, 1, 1, 1});

        // Points de contrôle
        utils::draw_disk(P0, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P1, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P2, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P3, 0.01f, {1, 0, 1, 1});

        // Lignes de contrôle
        utils::draw_line(P0, P1, 0.002f, {1, 1, 1, 0.2f});
        utils::draw_line(P1, P2, 0.002f, {1, 1, 1, 0.2f});
        utils::draw_line(P2, P3, 0.002f, {1, 1, 1, 0.2f});

        // Particules rouges
        for (auto& pt : particles)
        {
            utils::draw_disk(pt, 0.005f, {1.f, 0.f, 0.f, 0.8f});
        }
    }
}
