#include "mouseController.hpp"

void wme::MouseRotationController::rotateView(GLFWwindow* window, double dx, double dy, float dt, WmeGameObject& gameObject)
{
	glm::vec3 rotate{ 0 };

	rotate.y -= static_cast<float>(dx);
	rotate.x += static_cast<float>(dy);

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);

	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
}
