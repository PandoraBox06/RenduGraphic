#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>

struct Particle
{
    glm::vec2 position;
    glm::vec3 color;

    Particle(glm::vec2 pos) : position(pos), color(1.f, 1.f, 1.f) {}

    float radius() const { return 0.01f; }
};

bool is_valid(glm::vec2 candidate, float min_dist, std::vector<glm::vec2> const &points)
{
    for (const auto &p : points)
    {
        if (glm::distance(p, candidate) < min_dist)
            return false;
    }
    return true;
}

glm::vec2 random_point_around(glm::vec2 origin, float min_dist)
{
    float radius = utils::rand(min_dist, 2.f * min_dist); // rayon aléatoire
    float angle = utils::rand(0.f, 2.f * glm::pi<float>());
    return origin + radius * glm::vec2(std::cos(angle), std::sin(angle));
}

int main()
{
    gl::init("Poisson Disk Sampling");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glm::vec2 center = {0.f, 0.f};
    float disk_radius = 1.f;
    float min_dist = 0.03f;
    int max_attempts = 30;

    std::vector<glm::vec2> points;
    std::vector<glm::vec2> active;

    // Premier point aléatoire dans le disque
    while (true)
    {
        glm::vec2 p = center + utils::rand(0.f, disk_radius) * glm::normalize(glm::vec2{utils::rand(-1.f, 1.f), utils::rand(-1.f, 1.f)});
        if (glm::length(p - center) <= disk_radius)
        {
            points.push_back(p);
            active.push_back(p);
            break;
        }
    }

    while (!active.empty())
    {
        int idx = utils::rand(0, static_cast<int>(active.size())); // index aléatoire dans active
        glm::vec2 current = active[idx];
        bool found = false;

        for (int i = 0; i < max_attempts; ++i)
        {
            glm::vec2 candidate = random_point_around(current, min_dist);
            if (glm::length(candidate - center) > disk_radius)
                continue;

            if (is_valid(candidate, min_dist, points))
            {
                points.push_back(candidate);
                active.push_back(candidate);
                found = true;
                break;
            }
        }

        if (!found)
        {
            // Aucune position valide trouvée autour de ce point
            active.erase(active.begin() + idx);
        }
    }

    // Convertir en particules
    std::vector<Particle> particles;
    for (auto &p : points)
        particles.emplace_back(p);

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
