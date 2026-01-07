#pragma once

#include "Actor.h"
#include "RenderComponents.h"

class CubeActor : public Actor
{
public:
	explicit CubeActor(int id, Game* game);

	void Init();
	void Update(float deltaTime);
};

