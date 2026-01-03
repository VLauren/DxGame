#include "CameraActor.h"
#include "../Game.h"

CameraActor::CameraActor(int id, Game* game) : Actor(id, game)
{
}

void CameraActor::Init()
{
	auto self = shared_from_this();
	auto cameraComponent = std::make_shared<CameraComponent>(self, m_game->GetScene());

	AddComponent(cameraComponent);

	Actor::Init();
}

void CameraActor::Update(float deltaTime)
{
	m_position.x += deltaTime/2;

	Actor::Update(deltaTime);
}


