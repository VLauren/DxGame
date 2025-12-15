#pragma once

#include <memory>

class Actor;

class ActorComponent
{
protected:
	std::shared_ptr<Actor> m_pOwner;

public:
	virtual ~ActorComponent() { m_pOwner.reset(); }

	virtual void VInit() = 0;
	virtual void VUpdate(float deltaTime) {}

private:
	void SetOwner(std::shared_ptr<Actor> pOwner) { m_pOwner = pOwner; }
};

