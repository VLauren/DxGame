#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "Actors/Actor.h"
#include "Actors/CollisionComponents.h"

class Scene;

class Game
{
public:
	explicit Game(Scene* scene, GLFWwindow* window);

	void Init();
	void Destroy();
	void Update(float deltaTime);

	void AddActor(std::shared_ptr<Actor> actor);
	void RemoveActor(std::shared_ptr<Actor> actor);

	template<typename T>
	std::shared_ptr<T> FindActor()
	{
		for (auto& actor : m_actors)
		{
			if (auto derived = std::dynamic_pointer_cast<T>(actor))
				return derived;
		}
		return nullptr;
	}

	std::vector<std::shared_ptr<Actor>> GetActors() { return m_actors; }
	Scene* GetScene() { return m_scene; }
	GLFWwindow* GetWindow() { return m_window; }

	int NextId() { return m_idCount++; }

	void RegisterCollider(std::weak_ptr<CollisionComponent> c);
	void UnregisterCollider(std::weak_ptr<CollisionComponent> c);

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
    Scene* m_scene;
	GLFWwindow* m_window;

	int m_idCount;

	std::vector<std::weak_ptr<CollisionComponent>> m_colliders;

	void CheckCollisions();
};

