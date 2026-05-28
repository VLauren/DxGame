// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include "CollisionComponents.h"
#include "Actor.h"
#include "../Game.h"

void CollisionComponent::VInit() 
{
	m_pOwner.get()->GetGame()->RegisterCollider(weak_from_this());
}

void AABBCollisionComponent::VUpdate()
{

}

bool AABBCollisionComponent::CheckCollision(const CollisionComponent& other, CollisionResult* result)
{
	using namespace DirectX;

	if (const auto otherAABB = dynamic_cast<const AABBCollisionComponent*>(&other))
	{
		// AABB vs AABB
		// -------------

		float minAX = m_pOwner.get()->GetPosition().x + m_center.x - m_extents.x;
		float minAY = m_pOwner.get()->GetPosition().y + m_center.y - m_extents.y;
		float minAZ = m_pOwner.get()->GetPosition().z + m_center.z - m_extents.z;
		float maxAX = m_pOwner.get()->GetPosition().x + m_center.x + m_extents.x;
		float maxAY = m_pOwner.get()->GetPosition().y + m_center.y + m_extents.y;
		float maxAZ = m_pOwner.get()->GetPosition().z + m_center.z + m_extents.z;

		float minBX = otherAABB->m_pOwner.get()->GetPosition().x + otherAABB->m_center.x - otherAABB->m_extents.x;
		float minBY = otherAABB->m_pOwner.get()->GetPosition().y + otherAABB->m_center.y - otherAABB->m_extents.y;
		float minBZ = otherAABB->m_pOwner.get()->GetPosition().z + otherAABB->m_center.z - otherAABB->m_extents.z;
		float maxBX = otherAABB->m_pOwner.get()->GetPosition().x + otherAABB->m_center.x + otherAABB->m_extents.x;
		float maxBY = otherAABB->m_pOwner.get()->GetPosition().y + otherAABB->m_center.y + otherAABB->m_extents.y;
		float maxBZ = otherAABB->m_pOwner.get()->GetPosition().z + otherAABB->m_center.z + otherAABB->m_extents.z;

		// Overlap check
		float dx = std::min(maxAX, maxBX) - std::max(minAX, minBX);
		float dy = std::min(maxAY, maxBY) - std::max(minAY, minBY);
		float dz = std::min(maxAZ, maxBZ) - std::max(minAZ, minBZ);
		if (dx <= 0)
			return false;
		if (dy <= 0)
			return false;
		if (dz <= 0)
			return false;

		// Normal and penetration depth
		if (dx <= dy && dx <= dz)
		{
			result->penetrationDepth = dx;
			result->normal = (minAX < minBX) ? XMFLOAT3{ 1, 0, 0 } : XMFLOAT3{ -1, 0, 0 };
		}
		else if (dy <= dz)
		{
			result->penetrationDepth = dy;
			result->normal = (minAY < minBY) ? XMFLOAT3{ 0, 1, 0 } : XMFLOAT3{ 0, -1, 0 };
		}
		else
		{
			result->penetrationDepth = dz;
			result->normal = (minAZ < minBZ) ? XMFLOAT3{ 0, 0, 1 } : XMFLOAT3{ 0, 0, -1 };
		}

		// Contact point
		float cx = (std::max(minAX, minBX) + std::min(maxAX, maxBX)) * 0.5f;
		float cy = (std::max(minAY, minBY) + std::min(maxAY, maxBY)) * 0.5f;
		float cz = (std::max(minAZ, minBZ) + std::min(maxAZ, maxBZ)) * 0.5f;
		result->contactPoint = { cx, cy, cz };

		result->otherActor = otherAABB->m_pOwner;
		result->valid = true;

		return true;
	}

	return false;
}
