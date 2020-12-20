#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>

namespace gamestart
{

    struct TransformComponent
    {
        glm::vec3 position;
        glm::vec4 rotation;
    };

} // namespace gamestart

#endif // TRANSFORMCOMPONENT_H
