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

class CollisionComponent : public ActorComponent, public std::enable_shared_from_this<CollisionComponent>
{
public:
	explicit CollisionComponent(std::shared_ptr<Actor> owner) : ActorComponent(std::move(owner)) {}

	virtual void VInit();
	virtual void VUpdate() = 0;
	virtual bool CheckCollision(const CollisionComponent& other, CollisionResult* result) = 0;
};

class  AABBColisionComponent
{

};

