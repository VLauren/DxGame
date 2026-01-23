#pragma once

#include "Actor.h"

class Player : public Actor
{
public:
	explicit Player(int id, Game* game) : Actor(id, game) {}

	void Init();
	void Update(float deltaTime);

	void OnCollision(Actor* other, const CollisionResult& result);

private:
	const float moveSpeed = 5;
};

