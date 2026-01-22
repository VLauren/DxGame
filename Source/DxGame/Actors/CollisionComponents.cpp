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

	if (const auto otherAABB = dynamic_cast<const AABBCollisionComponent*>(&other))
	{
		// AABB vs AABB
		float minAX = m_pOwner.get()->GetPosition().x + m_center.x;
		float minAY = m_pOwner.get()->GetPosition().y + m_center.y;
		float minAZ = m_pOwner.get()->GetPosition().z + m_center.z;
		float minBX = otherAABB->m_pOwner.get()->GetPosition().x + otherAABB->m_center.x;
		float minBY = otherAABB->m_pOwner.get()->GetPosition().y + otherAABB->m_center.y;
		float minBZ = otherAABB->m_pOwner.get()->GetPosition().z + otherAABB->m_center.z;
		float maxAX = m_pOwner.get()->GetPosition().x + m_center.x + m_extents.x;
		float maxAY = m_pOwner.get()->GetPosition().y + m_center.y + m_extents.y;
		float maxAZ = m_pOwner.get()->GetPosition().z + m_center.z + m_extents.x;
		float maxBX = otherAABB->m_pOwner.get()->GetPosition().x + otherAABB->m_center.x + otherAABB->m_extents.x;
		float maxBY = otherAABB->m_pOwner.get()->GetPosition().y + otherAABB->m_center.y + otherAABB->m_extents.y;
		float maxBZ = otherAABB->m_pOwner.get()->GetPosition().z + otherAABB->m_center.z + otherAABB->m_extents.z;

		if (std::min(maxAX, maxBX) - std::max(minAX, minBX) < 0)
			return false;
		if (std::min(maxAY, maxBY) - std::max(minAY, minBY) < 0)
			return false;
		if (std::min(maxAZ, maxBZ) - std::max(minAZ, minBZ) < 0)
			return false;

		printf("aabb overlap\n");
		return true;
	}

	return false;
}
