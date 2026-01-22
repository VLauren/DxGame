#pragma once

#include <memory>

class Actor;

class ActorComponent
{
protected:
	std::shared_ptr<Actor> m_pOwner;

public:
	ActorComponent(std::shared_ptr<Actor> owner) { m_pOwner = owner; }
	virtual ~ActorComponent() { m_pOwner.reset(); }

	virtual void VInit() = 0;
	virtual void VUpdate(float deltaTime) {}
	virtual std::weak_ptr<Actor> GetOwner() { return std::weak_ptr(m_pOwner); }

private:
	void SetOwner(std::shared_ptr<Actor> pOwner) { m_pOwner = pOwner; }
};

