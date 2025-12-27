#include "CubeActor.h"
#include "../Game.h"

CubeActor::CubeActor(int id, Game* game) : Actor(id, game)
{
}

void CubeActor::Init()
{
	auto self = shared_from_this();
	auto cubeComponent = std::make_shared<CubeRenderComponent>(self, m_game->GetScene());

	AddComponent(cubeComponent);

	Actor::Init();
}
