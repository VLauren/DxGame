#pragma once

#include "Actor.h"
#include "RenderComponents.h"

class Player : public Actor
{
public:
	explicit Player(int id, Game* game) : Actor(id, game) {}

	void Init();
	void Update(float deltaTime);

	void OnCollision(Actor* other, const CollisionResult& result);

private:
	const float moveSpeed = 5;
	
	std::weak_ptr<MeshRenderComponent> m_modelIdle;
	std::weak_ptr<MeshRenderComponent> m_modelRun;
};

