#pragma once

#include "Actor.h"
#include "CameraComponent.h"

class CameraActor : public Actor
{
public:
	explicit CameraActor(int id, Game* game);

	void Init();
	void Update(float deltaTime);

};

