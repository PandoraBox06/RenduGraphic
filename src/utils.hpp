#pragma once
#include "glm/glm.hpp"
#include <vector>

namespace utils
{
    float rand(float min, float max);

    void draw_disk(glm::vec2 position, float radius, glm::vec4 const& color);
    void draw_line(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 const& color);
    void draw_polyline(std::vector<glm::vec2> const& points, bool closed, float thickness, glm::vec4 const& color);
}
