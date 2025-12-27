#pragma once

#include <string>
#include <memory>
#include <DirectXMath.h>

#include <vector>

class Scene;

class SceneNode
{
public:
	SceneNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix);

	virtual void VSetTransform(const DirectX::XMMATRIX& worldMatrix) { m_worldMatrix = worldMatrix; }
	virtual DirectX::XMMATRIX VGetTransform() { return  m_worldMatrix; }

	virtual void VPreRender(Scene* pScene) {};
	virtual void VRender(Scene* pScene) {};
	virtual void VRenderChildren(Scene* pScene);
	virtual void VPostRender(Scene* pScene) {};

	virtual bool VAddChild(std::shared_ptr<SceneNode> child);
	virtual bool VRemoveChild(int actorId);

	virtual ~SceneNode() {}

protected:
	int m_actorId;
	std::string m_name;
	DirectX::XMMATRIX m_worldMatrix;

	std::vector<std::shared_ptr<SceneNode>> m_children;
	SceneNode* m_pParent;
};

