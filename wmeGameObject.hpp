#pragma once

#include "wmeModel.hpp"

#include <memory>

namespace wme
{
	struct Transform2dObject
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() 
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotationMat * scaleMat; 
		}
	};

	class WmeGameObject
	{
	public:
		using id_t = unsigned int;

		static WmeGameObject createGameObject() 
		{
			static id_t currentId = 0;
			return WmeGameObject(currentId++);
		}

		WmeGameObject(const WmeGameObject&) = delete;
		WmeGameObject(WmeGameObject&&) = default;
		WmeGameObject& operator= (const WmeGameObject&) = delete;
		WmeGameObject& operator= (WmeGameObject&&) = default;

		const id_t getId() { return id; }

		std::shared_ptr<WmeModel> model;
		glm::vec3 color{};
		Transform2dObject transform2d{};

	private:
		WmeGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}