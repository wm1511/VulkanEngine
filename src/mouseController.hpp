#pragma once

#include "wmeGameObject.hpp"
#include "wmeWindow.hpp"

namespace wme
{
    class MouseRotationController
    {
    public:
        void rotateView(GLFWwindow* window, double dx, double dy, float dt, WmeGameObject& gameObject);

        float lookSpeed{ 2.f };
    };
}