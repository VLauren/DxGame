#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>

#include <dxgi1_3.h>
#include <vector>

class SceneNode;

class Scene
{

public:
	Scene();
	~Scene() {}

	void OnRender();

	bool AddChild(int actorId, std::shared_ptr<SceneNode> child);

	bool RemoveChild(int actorId);

protected:
	std::shared_ptr<SceneNode> m_root;
	
};

