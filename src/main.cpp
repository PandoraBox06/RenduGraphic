#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <cmath>

glm::vec2 P0 = {-0.5f, -0.2f};
glm::vec2 P1 = { 0.5f,  0.3f};

glm::vec2 de_casteljau(glm::vec2 p0, glm::vec2 p1, float t)
{
    return (1 - t) * p0 + t * p1;
}

int main()
{
    gl::init("Bézier Degré 1 - Casteljau");
    gl::maximize_window();

    float t = 0.f;
    bool forward = true;

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Animation douce de t entre 0 et 1
        float dt = 0.01f;
        if (forward)
            t += dt;
        else
            t -= dt;

        if (t >= 1.f) { t = 1.f; forward = false; }
        if (t <= 0.f) { t = 0.f; forward = true; }

        // Point interpolé via Casteljau
        glm::vec2 interpolated = de_casteljau(P0, P1, t);

        // Affichage des éléments
        utils::draw_disk(P0, 0.01f, {1.f, 0.f, 1.f, 1.f}); // violet
        utils::draw_disk(P1, 0.01f, {1.f, 0.f, 1.f, 1.f}); // violet
        utils::draw_line(P0, P1, 0.004f, {1.f, 1.f, 1.f, 1.f}); // blanc
        utils::draw_disk(interpolated, 0.01f, {1.f, 1.f, 0.f, 1.f}); // jaune (Casteljau)
    }
}
