#include "RenderComponents.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include "../Scene.h"
#include "../Graphics.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>

struct VertexPositionColor
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT3 normal;
};

struct VertexNormalUV
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
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

#ifdef _DEBUG
	std::wstring basePath = L"../Shaders/";
#else
	std::wstring basePath = L"";
#endif

	ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
	m_vertexShader = Graphics::CreateVertexShader(basePath + L"Main.vs.hlsl", vertexShaderBlob);
	m_pixelShader = Graphics::CreatePixelShader(basePath + L"Main.ps.hlsl");

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

void TextureCubeRenderComponent::VInit()
{
	RenderComponent::VInit();

	auto device = Graphics::GetDevice();
	
#ifdef _DEBUG
	std::wstring basePath = L"../Shaders/";
#else
	std::wstring basePath = L"";
#endif

	ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
	m_vertexShader = Graphics::CreateVertexShader(basePath + L"Textured.vs.hlsl", vertexShaderBlob);
	m_pixelShader = Graphics::CreatePixelShader(basePath + L"Textured.ps.hlsl");

	assert(m_vertexShader != nullptr && "Error creating vertex shader from file");
	assert(m_pixelShader != nullptr && "Error creating pixel shader from file");

	constexpr D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexNormalUV, position),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexNormalUV, normal),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			0,
			offsetof(VertexNormalUV, uv),
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

	static constexpr VertexNormalUV cubeVerts[24] =
	{
		{ { 1,-1,-1}, {1,0,0}, {0,1} },
		{ { 1, 1,-1}, {1,0,0}, {1,1} },
		{ { 1, 1, 1}, {1,0,0}, {1,0} },
		{ { 1,-1, 1}, {1,0,0}, {0,0} },

		{ {-1,-1, 1}, {-1,0,0}, {0,1} },
		{ {-1, 1, 1}, {-1,0,0}, {1,1} },
		{ {-1, 1,-1}, {-1,0,0}, {1,0} },
		{ {-1,-1,-1}, {-1,0,0}, {0,0} },

		{ {-1, 1,-1}, {0,1,0}, {0,1} },
		{ { 1, 1,-1}, {0,1,0}, {1,1} },
		{ { 1, 1, 1}, {0,1,0}, {1,0} },
		{ {-1, 1, 1}, {0,1,0}, {0,0} },

		{ {-1,-1, 1}, {0,-1,0}, {0,1} },
		{ { 1,-1, 1}, {0,-1,0}, {1,1} },
		{ { 1,-1,-1}, {0,-1,0}, {1,0} },
		{ {-1,-1,-1}, {0,-1,0}, {0,0} },

		{ {-1,-1, 1}, {0,0,1}, {0,1} },
		{ {-1, 1, 1}, {0,0,1}, {1,1} },
		{ { 1, 1, 1}, {0,0,1}, {1,0} },
		{ { 1,-1, 1}, {0,0,1}, {0,0} },

		{ { 1,-1,-1}, {0,0,-1}, {0,1} },
		{ { 1, 1,-1}, {0,0,-1}, {1,1} },
		{ {-1, 1,-1}, {0,0,-1}, {1,0} },
		{ {-1,-1,-1}, {0,0,-1}, {0,0} }
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
	geometryDesc.vertexStride = sizeof(VertexNormalUV);
	geometryDesc.vertexCount = 24;
	geometryDesc.vertexData = cubeVerts;
	geometryDesc.indexCount = 36;
	geometryDesc.indexData = cubeIdx;

	ComPtr<ID3D11Texture2D> tex;
	ComPtr<ID3D11ShaderResourceView> srv;

	// Texture generation, checkerboard pattern
	// constexpr int TEX = 128;
	// std::vector<UINT32> pixels(TEX* TEX);
	// auto checker = [&](int x, int y, uint32_t a, uint32_t b)
		// {
			// return ((x / 32 + y / 32) % 2) ? a : b;
		// };
	// for (int y = 0; y < TEX; ++y)
		// for (int x = 0; x < TEX; ++x)
			// pixels[y * TEX + x] = checker(x, y, 0xFF003366, 0xFFCCCCCC);

	constexpr int TEX = 128;
	constexpr int border = 6;
	std::vector<UINT32> pixels(TEX* TEX);

	std::srand(static_cast<unsigned>(std::time(nullptr)));

	for (int y = 0; y < TEX; ++y)
		for (int x = 0; x < TEX; ++x)
		{
			float dark = 0.10f * std::pow((std::rand() % 100) / 100.0f, 2);
			float edge = 0.50 * !(x<border || y<border || (x + border)>TEX || (y + border)>TEX) + 0.5f;
			float grey = std::min(1 - dark, edge);
			uint8_t v = static_cast<uint8_t>(grey * 255.0f);

			pixels[y * TEX + x] = 0xFF000000u | (v << 16) | (v << 8) | v;
		}


	D3D11_TEXTURE2D_DESC td = {};
	td.Width = td.Height = TEX;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc = { 1, 0 };
	td.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	td.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA init = {};
	init.pSysMem = pixels.data();
	init.SysMemPitch = TEX * sizeof(uint32_t);

	device->CreateTexture2D(&td, &init, tex.GetAddressOf());
	device->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());

	// CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
	CD3D11_SAMPLER_DESC samplDesc = {};
	samplDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ComPtr<ID3D11SamplerState> sampler;
	device->CreateSamplerState(&samplDesc, sampler.GetAddressOf());

	m_diffuseSRV = srv;
	m_sampler = sampler;

	Graphics::GetDeviceContext()->PSSetShaderResources(0, 1, m_diffuseSRV.GetAddressOf());
	Graphics::GetDeviceContext()->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

	node->SetGeometry(geometryDesc);
	node->VLoadResources(m_scene);
	node->SetShadersAndLayout(m_vertexShader, m_pixelShader, m_vertexLayout);

	m_sceneNode = node;
}

void LightComponent::VInit(DirectX::XMFLOAT3 colour, float intensity, std::array<float,3> attenuation)
{
	m_colour = colour;
	auto node = std::make_shared<LightNode>(m_pOwner->GetId(), "light node", DirectX::XMMatrixIdentity(), colour, intensity, attenuation);
	m_scene->AddChild(m_pOwner->GetId(), node);
	node->VLoadResources(m_scene);
	m_sceneNode = node;
}

void LightComponent::VUpdate(float deltaTime)
{
	RenderComponent::VUpdate(deltaTime);

	if (auto node = m_sceneNode.lock())
		if (auto light = std::dynamic_pointer_cast<LightNode, SceneNode>(node))
			light->SetColour(m_colour);
}

void LightComponent::SetColour(const DirectX::XMFLOAT3& colour)
{
	m_colour = colour;
}
