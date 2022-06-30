#pragma once

#include "wmeModel.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

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

	struct PointLightComponent
	{
		float lightIntensity = 1.0f;
	};

	class WmeGameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, WmeGameObject>;

		static WmeGameObject createGameObject() 
		{
			static id_t currentId = 0;
			return WmeGameObject(currentId++);
		}

		static WmeGameObject makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		WmeGameObject(const WmeGameObject&) = delete;
		WmeGameObject(WmeGameObject&&) = default;
		WmeGameObject& operator= (const WmeGameObject&) = delete;
		WmeGameObject& operator= (WmeGameObject&&) = default;

		const id_t getId() { return id; }

		glm::vec3 color{};
		TransformComponent transform{};

		std::shared_ptr<WmeModel> model;
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		WmeGameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}