#include "CubeActor.h"

CubeActor::CubeActor(int id) : Actor(id)
{
}

void CubeActor::Init()
{
	auto self = shared_from_this();
	auto cubeComponent = std::make_shared<CubeRenderComponent>(self);
	AddComponent(cubeComponent);

	Actor::Init();
}
