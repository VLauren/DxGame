#include "SceneNode.h"

SceneNode::SceneNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix)
{
	m_actorId = actorId;
	m_name = name;
	m_worldMatrix = worldMatrix;
}

void SceneNode::VRenderChildren(Scene* pScene) 
{
	for(auto child : m_children)
		child->VPreRender(pScene);
	for(auto child : m_children)
		child->VRender(pScene);
	for(auto child : m_children)
		child->VPostRender(pScene);
}

bool SceneNode::VAddChild(std::shared_ptr<SceneNode> child) 
{
	m_children.emplace_back(child);
	return true;
}

bool SceneNode::VRemoveChild(int actorId) 
{
	return true;
}
