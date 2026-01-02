#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>

#include <dxgi1_3.h>
#include <vector>

class SceneNode;
class CameraNode;

class Scene
{

public:
	Scene();
	~Scene() {}

	void OnRender();
	bool AddChild(int actorId, std::shared_ptr<SceneNode> child);
	bool RemoveChild(int actorId);

	DirectX::XMMATRIX GetViewProjMatrix() { return m_viewProj; }
	void SetViewProjMatrix(DirectX::XMMATRIX viewProj) { m_viewProj = viewProj; }

protected:
	std::shared_ptr<SceneNode> m_root;
	std::shared_ptr<CameraNode> m_cameraNode;

    DirectX::XMMATRIX m_viewProj = DirectX::XMMatrixIdentity();
};

