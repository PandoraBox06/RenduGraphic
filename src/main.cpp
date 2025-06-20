#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>

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

struct CircleObstacle
{
    glm::vec2 center;
    float radius;
};

int main()
{
    gl::init("Particles Bounce on Circles");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    std::vector<Particle> particles;
    for (int i = 0; i < 100; ++i)
        particles.emplace_back();

    // Create circle obstacles
    std::vector<CircleObstacle> circles;
    for (int i = 0; i < 5; ++i)
    {
        glm::vec2 center{
            utils::rand(-gl::window_aspect_ratio(), +gl::window_aspect_ratio()),
            utils::rand(-1.f, +1.f),
        };
        float radius = utils::rand(0.05f, 0.1f);
        circles.push_back({center, radius});
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw circle obstacles
        for (auto const &circle : circles)
            utils::draw_disk(circle.center, circle.radius, {0.f, 1.f, 1.f, 1.f}); // Cyan

        for (auto &particle : particles)
        {
            glm::vec2 prev = particle.position;

            // Apply air resistance
            glm::vec2 force = -particle.velocity * gl::delta_time_in_seconds();
            particle.velocity += force * gl::delta_time_in_seconds();

            glm::vec2 new_pos = particle.position + particle.velocity * gl::delta_time_in_seconds();

            for (auto const &circle : circles)
            {
                glm::vec2 to_particle = new_pos - circle.center;
                float dist_sq = glm::dot(to_particle, to_particle);
                float combined_radius = circle.radius + particle.radius();

                if (dist_sq < combined_radius * combined_radius)
                {
                    glm::vec2 normal = glm::normalize(to_particle);
                    particle.velocity = reflect(particle.velocity, normal);
                    new_pos = circle.center + normal * combined_radius;
                }
            }

            particle.position = new_pos;

            utils::draw_disk(particle.position, particle.radius(), glm::vec4(particle.color, 1.f));
        }
    }
}
