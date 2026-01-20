#include "CollisionComponents.h"
#include "Actor.h"
#include "../Game.h"

void CollisionComponent::VInit() 
{
	m_pOwner.get()->GetGame()->RegisterCollider(weak_from_this());
}
