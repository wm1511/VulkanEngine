#pragma once

#include "wmeGameObject.hpp"
#include "wmeWindow.hpp"

namespace wme
{
	class KeyboardMovementController
	{
	public:
		struct KeyMappings
		{
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int releaseCursor = GLFW_KEY_ESCAPE;
            int captureCursor = GLFW_KEY_ENTER;
		};

        void moveInPlaneXZ(GLFWwindow* window, float dt, WmeGameObject& gameObject);
        int controlCursorMode(GLFWwindow* window);

        KeyMappings keys{};
        float moveSpeed{ 2.f };
	};
}