#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>

struct Particle
{
    glm::vec2 position;
    glm::vec3 color;

    Particle(glm::vec2 pos)
        : position(pos), color(1.f, 1.f, 1.f) {}

    float radius() const { return 0.005f; }
};

int main()
{
    gl::init("Particules!");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glm::vec2 center{0.f, 0.f};
    float radius = 1.f;

    std::vector<Particle> particles;
    for (int i = 0; i < 1000; ++i)
    {
        float r = radius * std::sqrt(utils::rand(0.f, 1.f));
        float theta = utils::rand(0.f, 2.f * glm::pi<float>());
        glm::vec2 pos = center + r * glm::vec2(std::cos(theta), std::sin(theta));
        particles.emplace_back(pos);
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
