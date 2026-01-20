#pragma once

#include "ActorComponent.h"
#include <DirectXMath.h>

struct CollisionResult
{
	std::weak_ptr<Actor> otherActor;
	DirectX::XMFLOAT3 contactPoint;
	DirectX::XMFLOAT3 normal;
	float penetrationDepth;
	bool valid;
};

class CollisionComponent : public ActorComponent
{
public:
	explicit CollisionComponent(std::shared_ptr<Actor> owner) : ActorComponent(std::move(owner)) {}

	virtual void VUpdate() = 0;
	virtual bool CheckCollision(CollisionComponent other, const CollisionResult& result) = 0;
};

class  AABBColisionComponent
{

};

