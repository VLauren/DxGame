#include "CubeActor.h"
#include "../Game.h"

CubeActor::CubeActor(int id, Game* game) : Actor(id, game)
{
	w = 1;
	h = 1;
	d = 1;
}

CubeActor::CubeActor(int id, Game* game, const std::array<float,3>& scale) : CubeActor(id, game)
{
	w = scale[0];
	h = scale[1];
	d = scale[2];
}

void CubeActor::Init()
{
	auto self = shared_from_this();
	auto cubeComponent = std::make_shared<TextureCubeRenderComponent>(self, m_game->GetScene(), w, h, d);

	AddComponent(cubeComponent);

	Actor::Init();
}

void CubeActor::Update(float deltaTime)
{
	Actor::Update(deltaTime);
}
