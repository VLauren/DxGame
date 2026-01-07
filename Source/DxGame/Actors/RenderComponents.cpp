#include "RenderComponents.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include "../Scene.h"
#include "../Graphics.h"


struct VertexPositionColor
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT3 normal;
};

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

void RenderComponent::VInit()
{
}

void RenderComponent::VUpdate(float deltaTime)
{
	using namespace DirectX;

	XMFLOAT3 p = m_pOwner->GetPosition();
	XMFLOAT3 r = m_pOwner->GetRotation();
	XMFLOAT3 s = m_pOwner->GetScale();
	XMMATRIX world = XMMatrixScalingFromVector(XMLoadFloat3(&s)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&r)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&p));

	if(auto node = m_sceneNode.lock())
		node->VSetTransform(world);
}


void CubeRenderComponent::VInit()
{
	RenderComponent::VInit();

	auto device = Graphics::GetDevice();

	ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
	m_vertexShader = Graphics::CreateVertexShader(L"../Shaders/Main.vs.hlsl", vertexShaderBlob);
	m_pixelShader = Graphics::CreatePixelShader(L"../Shaders/Main.ps.hlsl");

	assert(m_vertexShader != nullptr && "Error creating vertex shader from file");
	assert(m_pixelShader != nullptr && "Error creating pixel shader from file");

	constexpr D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexPositionColor, position),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexPositionColor, color),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexPositionColor, normal),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	if (FAILED(device->CreateInputLayout(
		vertexInputLayoutInfo,
		_countof(vertexInputLayoutInfo),
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		&m_vertexLayout)))
	{
		printf("D3D11: Failed to create default vertex input layout\n");
	}

	auto node = std::make_shared<ShaderMeshNode>(m_pOwner->GetId(), std::string("Cube render node"), DirectX::XMMatrixIdentity());
	m_scene->AddChild(m_pOwner->GetId(), node);

	constexpr VertexPositionColor cubeVerts[24] =
	{
		{ { 1,-1,-1}, {1.0f,0.5f,0.5f}, {1,0,0} },
		{ { 1, 1,-1}, {1.0f,0.5f,0.5f}, {1,0,0} },
		{ { 1, 1, 1}, {1.0f,0.5f,0.5f}, {1,0,0} },
		{ { 1,-1, 1}, {1.0f,0.5f,0.5f}, {1,0,0} },

		{ {-1,-1, 1}, {0.5f,1.0f,0.5f}, {-1,0,0} },
		{ {-1, 1, 1}, {0.5f,1.0f,0.5f}, {-1,0,0} },
		{ {-1, 1,-1}, {0.5f,1.0f,0.5f}, {-1,0,0} },
		{ {-1,-1,-1}, {0.5f,1.0f,0.5f}, {-1,0,0} },

		{ {-1, 1,-1}, {0.5f,0.5f,1.0f}, {0,1,0} },
		{ { 1, 1,-1}, {0.5f,0.5f,1.0f}, {0,1,0} },
		{ { 1, 1, 1}, {0.5f,0.5f,1.0f}, {0,1,0} },
		{ {-1, 1, 1}, {0.5f,0.5f,1.0f}, {0,1,0} },

		{ {-1,-1, 1}, {1.0f,1.0f,0.5f}, {0,-1,0} },
		{ { 1,-1, 1}, {1.0f,1.0f,0.5f}, {0,-1,0} },
		{ { 1,-1,-1}, {1.0f,1.0f,0.5f}, {0,-1,0} },
		{ {-1,-1,-1}, {1.0f,1.0f,0.5f}, {0,-1,0} },

		{ {-1,-1, 1}, {1.0f,0.5f,1.0f}, {0,0,1} },
		{ {-1, 1, 1}, {1.0f,0.5f,1.0f}, {0,0,1} },
		{ { 1, 1, 1}, {1.0f,0.5f,1.0f}, {0,0,1} },
		{ { 1,-1, 1}, {1.0f,0.5f,1.0f}, {0,0,1} },

		{ { 1,-1,-1}, {0.5f,1.0f,1.0f}, {0,0,-1} },
		{ { 1, 1,-1}, {0.5f,1.0f,1.0f}, {0,0,-1} },
		{ {-1, 1,-1}, {0.5f,1.0f,1.0f}, {0,0,-1} },
		{ {-1,-1,-1}, {0.5f,1.0f,1.0f}, {0,0,-1} }
	};

	constexpr unsigned short cubeIdx[36] =
	{
		0, 1, 2,    0, 2, 3,
		4, 5, 6,    4, 6, 7,
		8,10, 9,    8,11,10,
	   12,14,13,   12,15,14,
	   16,18,17,   16,19,18,
	   20,22,21,   20,23,22
	};

	ShaderMeshNode::GeometryDesc geometryDesc = {};
	geometryDesc.vertexStride = sizeof(VertexPositionColor);
	geometryDesc.vertexCount = 24;
	geometryDesc.vertexData = cubeVerts;
	geometryDesc.indexCount = 36;
	geometryDesc.indexData = cubeIdx;

	node->SetGeometry(geometryDesc);
	node->VLoadResources(m_scene);
	node->SetShadersAndLayout(m_vertexShader, m_pixelShader, m_vertexLayout);

	m_sceneNode = node;

}
