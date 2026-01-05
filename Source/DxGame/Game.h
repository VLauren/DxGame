#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "Actors/Actor.h"

class Scene;

class Game
{
public:
	explicit Game(Scene* scene, GLFWwindow* window);

	void Init();
	void Destroy();
	void Update(float deltaTime);

	void AddActor(std::shared_ptr<Actor> actor)
	{
		m_actors.emplace_back(actor);
	}

	void RemoveActor(std::shared_ptr<Actor> actor)
	{
		std::erase(m_actors, actor);
	}

	std::vector<std::shared_ptr<Actor>> GetActors() { return m_actors; }
	Scene* GetScene() { return m_scene; }
	GLFWwindow* GetWindow() { return m_window; }

private:
	std::vector<std::shared_ptr<Actor>> m_actors;
    Scene* m_scene;
	GLFWwindow* m_window;
};

