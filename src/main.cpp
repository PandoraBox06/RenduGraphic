#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>

struct Particle
{
    glm::vec2 position;
    glm::vec3 color;

    Particle(glm::vec2 pos)
        : position(pos), color(1.f, 1.f, 1.f) // blanc pur
    {
    }

    float radius() const { return 0.005f; }
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Rectangle : centré en (0.5, 0), largeur 0.5, hauteur 1.5
    float rectCenterX = 0.5f;
    float rectWidth = 0.5f;
    float rectHeight = 1.5f;

    std::vector<Particle> particles;
    for (int i = 0; i < 1000; ++i)
    {
        float x = utils::rand(rectCenterX - rectWidth / 2.f, rectCenterX + rectWidth / 2.f);
        float y = utils::rand(-rectHeight / 2.f, rectHeight / 2.f);
        particles.emplace_back(glm::vec2{x, y});
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto const &particle : particles)
        {
            utils::draw_disk(particle.position, particle.radius(), glm::vec4(particle.color, 1.f));
        }
    }
}
