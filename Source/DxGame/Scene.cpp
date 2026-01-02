#include "Scene.h"
#include "SceneNodes.h"

Scene::Scene() 
{
	m_root = std::make_shared<SceneNode>(1, std::string("root node"), DirectX::XMMatrixIdentity());
	m_cameraNode = std::make_shared<CameraNode>(1, std::string("camera node"), DirectX::XMMatrixIdentity());
}

void Scene::OnRender()
{
	m_cameraNode->VRender(this);
	m_root->VRenderChildren(this);
}

bool Scene::AddChild(int actorId, std::shared_ptr<SceneNode> child)
{
	return m_root->VAddChild(child);
}

bool Scene::RemoveChild(int actorId)
{
	return m_root->VRemoveChild(actorId);
}
