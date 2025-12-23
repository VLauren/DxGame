#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>

#include <dxgi1_3.h>
#include <vector>

class SceneNode;

class Scene
{
protected:
	std::shared_ptr<SceneNode> m_root;

public:
	Scene() {}
	~Scene() {}

	void OnRender()
	{

	}

	bool AddChild(int actorId, std::shared_ptr<SceneNode> child)
	{
		// return m_root->VAddChild(child);
	}

	bool RemoveChild(int actorId)
	{
		// return m_root->VRemoveChild(child);
	}

	
};

