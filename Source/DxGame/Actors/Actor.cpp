#include "ActorComponent.h"
#include "Actor.h"

Actor::Actor(int id)
{
	m_id = id;
}

Actor::~Actor()
{
	assert(m_components.empty() && "Destroy() was not called");
}

void Actor::Init()
{
}

void Actor::Destroy()
{
	m_components.clear();
}

void Actor::Update(float deltaTime)
{
	// Update the actor components
	for(auto& component : m_components)
		component->VUpdate(deltaTime);
}

void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent)
{
	m_components.push_back(pComponent);
}
