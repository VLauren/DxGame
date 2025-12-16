#pragma once

#include <vector>
#include <memory>

#include "Actors/Actor.h"

class Game
{
	std::vector<std::shared_ptr<Actor>> m_actors;

public:
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
};

