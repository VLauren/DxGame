#pragma once
#include "ActorComponent.h"

class CubeRenderComponent : public ActorComponent
{
public:
	explicit CubeRenderComponent() : ActorComponent() {}

	void VInit()
	{

	}

	void VUpdate(float deltaTime) override
	{
		printf("cube render update\n");
	}
};

