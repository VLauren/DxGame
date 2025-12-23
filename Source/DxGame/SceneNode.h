#pragma once

#include <string>
#include <memory>
#include <DirectXMath.h>

#include <vector>

class Scene;

class SceneNode
{
public:
	SceneNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix)
	{
		m_actorId = actorId;
		m_name = name;
		m_worldMatrix = worldMatrix;
	}

	virtual void VSetTransform(const DirectX::XMMATRIX& worldMatrix) { m_worldMatrix = worldMatrix; }
	virtual DirectX::XMMATRIX VGetTransform() { return  m_worldMatrix; }

	virtual void VPreRender(Scene *pScene) = 0;
	virtual void VRender(Scene *pScene) = 0;
	virtual void VRenderChildren(Scene *pScene) = 0;
	virtual void VPostRender(Scene *pScene) = 0;

	virtual bool VAddChild(std::shared_ptr<SceneNode> child) = 0;
	virtual bool VRemoveChild(int actorId) = 0;

	virtual ~SceneNode() {}

protected:
	int m_actorId;
	std::string m_name;
	DirectX::XMMATRIX m_worldMatrix;

	std::vector<std::shared_ptr<SceneNode>> m_children;
	SceneNode* m_pParent;
};

