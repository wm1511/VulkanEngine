#include "mouseController.hpp"

#include <limits>

void wme::MouseRotationController::rotateView(GLFWwindow* window, double& cursorX, double& cursorY, float dt, WmeGameObject& gameObject)
{
	glm::vec3 rotate{ 0 };

	glfwGetCursorPos(window, &cursorX, &cursorY);
	glfwSetCursorPos(window, 0, 0);

	rotate.y += static_cast<float>(cursorX);
	rotate.x -= static_cast<float>(cursorY);

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		gameObject.transform.rotation += lookSpeed * dt * rotate;

	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
}
