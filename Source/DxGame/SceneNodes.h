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

public:

	struct FrameCB { uint32_t seed; uint32_t pad[3]; };

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
		DirectX::XMMATRIX viewProj;
	};

	struct GeometryDesc
	{
		int32_t vertexStride = 0;
		int32_t vertexCount = 0;
		const void* vertexData = nullptr;

		int32_t indexCount = 0;
		const void* indexData = nullptr;
	};

	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 normal;
	};

	ShaderMeshNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix) :
		SceneNode(actorId, name, worldMatrix) {}

	virtual void SetGeometry(const GeometryDesc& desc);
	virtual void VLoadResources(Scene* pScene);
	virtual void SetShadersAndLayout(
		ComPtr<ID3D11VertexShader> vs,
		ComPtr<ID3D11PixelShader> ps,
		ComPtr<ID3D11InputLayout> il);

protected:
	virtual void VPreRender(Scene* pScene);
	virtual void VPostRender(Scene* pScene);

private:
	GeometryDesc m_geometryDesc{};

    ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
    ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    // ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

	// uint32_t m_frame = 0;
};


class CameraNode : public SceneNode
{
public:
	CameraNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix) :
		SceneNode(actorId, name, worldMatrix) {}
	~CameraNode() {}

	virtual void VPreRender(Scene* pScene);
	virtual void VRender(Scene* pScene);
	virtual void VRenderChildren(Scene* pScene);
	virtual void VPostRender(Scene* pScene);

	// virtual bool VAddChild(std::shared_ptr<SceneNode> child);
	// virtual bool VRemoveChild(int actorId);

private:
	float m_aspectRatio;
	float m_fov;
	float m_nearZ;
	float m_farZ;

	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_projection;
};

class LightNode : public SceneNode
{
	struct LightConstantBuf
	{
		DirectX::XMFLOAT3 Pos;
		float _pad0; // 12 + 4 = 16
		DirectX::XMFLOAT3 Color;
		float _pad1; // 12 + 4 = 16
		float Intensity;
		float AttConst;
		float AttLin;
		float AttQuad;
	};

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	LightNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix) :
		SceneNode(actorId, name, worldMatrix) {}
	~LightNode() {}

	virtual void VLoadResources(Scene* pScene);
	virtual void VPreRender(Scene* pScene);
	virtual void VRender(Scene* pScene);

private:
	DirectX::XMFLOAT3 m_colour;
	float m_intensity;
	float m_attenuation[3];

	ComPtr<ID3D11Buffer> m_constantBuffer;
};

