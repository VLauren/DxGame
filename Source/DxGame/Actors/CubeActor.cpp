#include "CubeActor.h"

CubeActor::CubeActor(int id) : Actor(id)
{
	auto cubeComponent = std::make_shared<CubeRenderComponent>();
	AddComponent(cubeComponent);
}
