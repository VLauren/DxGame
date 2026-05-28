#pragma once

#include "Actor.h"
#include "CameraComponent.h"

class Player;

class CameraActor : public Actor
{
public:
	explicit CameraActor(int id, Game* game);

	void Init();
	void Update(float deltaTime);

private:
	std::weak_ptr<Player> m_playerRef;
	DirectX::XMFLOAT3 m_offset{ 0, 6, -9 };
};

