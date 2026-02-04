#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "Scene.h"
#include "SceneNodes.h"

Scene::Scene() 
{
	m_root = std::make_shared<SceneNode>(1, std::string("root node"), DirectX::XMMatrixIdentity());

	// m_light = std::make_shared<LightNode>(-1, "light node", DirectX::XMMatrixIdentity());
	// m_light->VLoadResources(this);
	// m_root->VAddChild(m_light);
}

void Scene::OnRender()
{
	if (m_cameraNode)
		m_cameraNode->VRender(this);
	else
		printf("No cam!\n");
	m_root->VRenderChildren(this);
}

bool Scene::AddChild(int actorId, std::shared_ptr<SceneNode> child)
{
	// save camera reference
	auto cam = std::dynamic_pointer_cast<CameraNode>(child);
	if (cam)
		m_cameraNode = cam;

	return m_root->VAddChild(child);
}

bool Scene::AddChildLast(int actorId, std::shared_ptr<SceneNode> child)
{
	return m_root->VAddChildLast(child);
}

bool Scene::RemoveChild(int actorId)
{
	return m_root->VRemoveChild(actorId);
}
