#pragma once

#include "wmeGameObject.hpp"
#include "wmeWindow.hpp"

namespace wme
{
    class MouseRotationController
    {
    public:
        void rotateView(GLFWwindow* window, double& cursorX, double& cursorY, float dt, WmeGameObject& gameObject);

        float lookSpeed{ .05f };
    };
}