#pragma once

#include "wmeModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace wme
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		glm::mat3 normalMatrix();
		glm::mat4 mat4();
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
		TransformComponent transform{};

	private:
		WmeGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}