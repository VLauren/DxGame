#pragma once

#include "Actor.h"
#include "RenderComponents.h"

class CubeActor : public Actor
{
public:
	explicit CubeActor(int id, Game* game);
	explicit CubeActor(int id, Game* game, const std::array<float,3>& scale);

	void Init();
	void Update(float deltaTime);
private:
	float w, h, d;
};

