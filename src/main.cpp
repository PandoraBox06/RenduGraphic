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

    std::vector<glm::vec2> circle;
    glm::vec2 center = {0.f, 0.f};
    float radius = 1.f;
    int segments = 256;

    for (int i = 0; i < segments; ++i)
    {
        float t = float(i) / float(segments);
        float angle = t * 2.f * glm::pi<float>();
        glm::vec2 point = center + radius * glm::vec2(std::cos(angle), std::sin(angle));
        circle.push_back(point);
    }

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        utils::draw_polyline(circle, true, 0.002f, {1.f, 1.f, 1.f, 1.f}); // cercle blanc
    }
}
