#include "Scene.h"
#include "SceneNode.h"

Scene::Scene() 
{
	// m_root = std::make_shared<SceneNode>(1, std::string("root node"), DirectX::XMMatrixIdentity);
}

void Scene::OnRender()
{
	// m_root->VRenderChildren(this);

	std::printf("scene render\n");
}

bool Scene::AddChild(int actorId, std::shared_ptr<SceneNode> child)
{
	return m_root->VAddChild(child);
}

bool Scene::RemoveChild(int actorId)
{
	return m_root->VRemoveChild(actorId);
}
