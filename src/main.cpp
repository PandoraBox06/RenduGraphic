#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

// === STRUCTURE PARTICULE ===
struct Particle
{
    glm::vec2 pos;
    glm::vec2 vel;
};

glm::vec2 P0 = {-0.8f, -0.6f};
glm::vec2 P1 = {-0.4f,  0.8f};
glm::vec2 P2 = { 0.4f,  0.8f};
glm::vec2 P3 = { 0.8f, -0.6f};

glm::vec2 de_casteljau(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
    glm::vec2 a = (1 - t) * p0 + t * p1;
    glm::vec2 b = (1 - t) * p1 + t * p2;
    glm::vec2 c = (1 - t) * p2 + t * p3;

    glm::vec2 d = (1 - t) * a + t * b;
    glm::vec2 e = (1 - t) * b + t * c;

    return (1 - t) * d + t * e;
}

// Approximation de la tangente : dérivée centrale
glm::vec2 bezier_tangent(float t, float eps = 1e-4f)
{
    glm::vec2 before = de_casteljau(P0, P1, P2, P3, glm::clamp(t - eps, 0.f, 1.f));
    glm::vec2 after  = de_casteljau(P0, P1, P2, P3, glm::clamp(t + eps, 0.f, 1.f));
    return glm::normalize(after - before);
}

// Normale en 2D : rotée tangente de 90° antihoraire
glm::vec2 normal_from_tangent(glm::vec2 tangent)
{
    return glm::normalize(glm::vec2(-tangent.y, tangent.x));
}

// Courbe pour affichage
std::vector<glm::vec2> compute_bezier_curve(int resolution = 64)
{
    std::vector<glm::vec2> curve;
    for (int i = 0; i <= resolution; ++i)
    {
        float t = static_cast<float>(i) / resolution;
        curve.push_back(de_casteljau(P0, P1, P2, P3, t));
    }
    return curve;
}

// Particules initialisées régulièrement avec vitesse normale
std::vector<Particle> spawn_particles_along_curve(int count, float speed = 0.2f)
{
    std::vector<Particle> particles;

    for (int i = 0; i < count; ++i)
    {
        float t = static_cast<float>(i) / (count - 1);
        glm::vec2 pos = de_casteljau(P0, P1, P2, P3, t);
        glm::vec2 tangent = bezier_tangent(t);
        glm::vec2 normal = normal_from_tangent(tangent);
        glm::vec2 vel = speed * normal;

        particles.push_back({pos, vel});
    }

    return particles;
}

int main()
{
    gl::init("Particules vitesse normale");
    gl::maximize_window();

    std::vector<glm::vec2> curve = compute_bezier_curve(100);
    std::vector<Particle> particles = spawn_particles_along_curve(100, 0.2f);

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Affiche courbe
        utils::draw_polyline(curve, false, 0.005f, {1, 1, 1, 1});

        // Affiche particules
        for (auto& p : particles)
        {
            // Mise à jour position avec vitesse
            float dt =  gl::delta_time_in_seconds();
            p.pos += p.vel * dt;
            utils::draw_disk(p.pos, 0.005f, {1.f, 0.f, 0.f, 0.8f});
        }

        // Points de contrôle
        utils::draw_disk(P0, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P1, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P2, 0.01f, {1, 0, 1, 1});
        utils::draw_disk(P3, 0.01f, {1, 0, 1, 1});
    }
}
