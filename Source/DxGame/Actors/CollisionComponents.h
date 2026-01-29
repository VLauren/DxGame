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

class  AABBCollisionComponent : public CollisionComponent
{
public:
	explicit AABBCollisionComponent(std::shared_ptr<Actor> owner, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 extents) 
		: CollisionComponent(std::move(owner)), m_center(center), m_extents(extents) {}

	virtual void VUpdate();

	virtual bool CheckCollision(const CollisionComponent& other, CollisionResult* result);

	DirectX::XMFLOAT3 GetExtents(){ return m_extents; }
	DirectX::XMFLOAT3 GetCenter(){ return m_center; }

private:
	DirectX::XMFLOAT3 m_center;
	DirectX::XMFLOAT3 m_extents;
};

