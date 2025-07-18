#include "glm/ext/scalar_constants.hpp"
#include "opengl-framework/opengl-framework.hpp"
#include "utils.hpp"
#include <vector>
#include <cmath>
#include <optional>

int main()
{
    gl::init("Courbes Paramétriques");
    gl::maximize_window();

    std::vector<glm::vec2> heart_points;
    int segments = 300;

    for (int i = 0; i < segments; ++i)
    {
        float t = glm::pi<float>() * 2.f * i / segments;

        float x = 16 * std::pow(std::sin(t), 3);
        float y = 13 * std::cos(t) - 5 * std::cos(2 * t) - 2 * std::cos(3 * t) - std::cos(4 * t);

        heart_points.emplace_back(glm::vec2{x, y} * 0.025f); // scale down
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        utils::draw_polyline(heart_points, true, 0.004f, {1.f, 1.f, 1.f, 1.f});
    }
}
