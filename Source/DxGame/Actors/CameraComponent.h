#pragma once

#include "ActorComponent.h"
#include "../SceneNodes.h"

class CameraComponent : public ActorComponent
{
public:
	explicit CameraComponent(std::shared_ptr<Actor> owner, class Scene* scene) : ActorComponent(std::move(owner)), m_scene(scene) {}

	void VInit();
	void VUpdate(float deltaTime);

private:

    std::weak_ptr<CameraNode> m_sceneNode;

    class Scene* m_scene;
};

