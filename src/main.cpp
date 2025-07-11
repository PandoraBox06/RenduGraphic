#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <optional>

struct Particle
{
    glm::vec2 position;
    glm::vec3 color;

    Particle(glm::vec2 pos) : position(pos), color(1.f, 1.f, 1.f) {}
    float radius() const { return 0.01f; }
};

// Helper to compute cell index
glm::ivec2 get_cell(glm::vec2 pos, float cell_size, glm::vec2 origin)
{
    glm::vec2 offset = pos - origin;
    return glm::ivec2(std::floor(offset.x / cell_size), std::floor(offset.y / cell_size));
}

int main()
{
    gl::init("Poisson Disk Sampling with Grid");
    gl::maximize_window();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Params
    glm::vec2 center = {0.f, 0.f};
    float radius = 1.f;
    float min_dist = 0.03f;
    float cell_size = min_dist / std::sqrt(2.f);
    int max_attempts = 30;

    // Bounding box of disk area
    glm::vec2 box_min = center - glm::vec2(radius);
    glm::vec2 box_max = center + glm::vec2(radius);
    glm::ivec2 grid_size = glm::ceil((box_max - box_min) / cell_size);

    // Grid: flat vector indexed as y * width + x
    std::vector<std::optional<glm::vec2>> grid(grid_size.x * grid_size.y, std::nullopt);
    auto in_bounds = [&](glm::ivec2 cell)
    {
        return cell.x >= 0 && cell.y >= 0 && cell.x < grid_size.x && cell.y < grid_size.y;
    };

    std::vector<glm::vec2> points;
    std::vector<glm::vec2> active;

    // First point
    while (true)
    {
        glm::vec2 p = center + utils::rand(0.f, radius) * glm::normalize(glm::vec2{utils::rand(-1.f, 1.f), utils::rand(-1.f, 1.f)});
        if (glm::length(p - center) <= radius)
        {
            points.push_back(p);
            active.push_back(p);
            grid[get_cell(p, cell_size, box_min).y * grid_size.x + get_cell(p, cell_size, box_min).x] = p;
            break;
        }
    }

    // Sampling loop
    while (!active.empty())
    {
        int idx = utils::rand(0, static_cast<int>(active.size()));
        glm::vec2 current = active[idx];
        bool found = false;

        for (int i = 0; i < max_attempts; ++i)
        {
            float angle = utils::rand(0.f, 2.f * glm::pi<float>());
            float r = utils::rand(min_dist, 2.f * min_dist);
            glm::vec2 candidate = current + r * glm::vec2(std::cos(angle), std::sin(angle));

            if (glm::length(candidate - center) > radius)
                continue;

            glm::ivec2 cell = get_cell(candidate, cell_size, box_min);
            bool valid = true;

            for (int y = -1; y <= 1 && valid; ++y)
            {
                for (int x = -1; x <= 1 && valid; ++x)
                {
                    glm::ivec2 neighbor = cell + glm::ivec2{x, y};
                    if (!in_bounds(neighbor))
                        continue;

                    auto &slot = grid[neighbor.y * grid_size.x + neighbor.x];
                    if (slot.has_value() && glm::distance(slot.value(), candidate) < min_dist)
                        valid = false;
                }
            }

            if (valid)
            {
                points.push_back(candidate);
                active.push_back(candidate);
                grid[cell.y * grid_size.x + cell.x] = candidate;
                found = true;
                break;
            }
        }

        if (!found)
            active.erase(active.begin() + idx);
    }

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
