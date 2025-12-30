#pragma once

#include <string>
#include <memory>
#include <vector>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <DirectXMath.h>

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

class ShaderMeshNode : public SceneNode
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct FrameCB { uint32_t seed; uint32_t pad[3]; };
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
		DirectX::XMMATRIX viewProj;
	};

public:

	ShaderMeshNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix) :
		SceneNode(actorId, name, worldMatrix) {}

	virtual void VPreRender(Scene* pScene);
	virtual void VRender(Scene* pScene);

	virtual void VLoadResources();
	virtual void SetShadersAndLayout(
		ComPtr<ID3D11VertexShader> vs,
		ComPtr<ID3D11PixelShader> ps,
		ComPtr<ID3D11InputLayout> il);

protected:

    ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
    ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

	uint32_t m_frame = 0;
};

