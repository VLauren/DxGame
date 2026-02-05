// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include "RenderComponents.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include "../Graphics/Scene.h"
#include "../Graphics/Graphics.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>

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
	geometryDesc.topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	node->SetGeometry(geometryDesc);
	node->VLoadResources(m_scene);
	node->SetShadersAndLayout(m_vertexShader, m_pixelShader, m_vertexLayout);

	m_sceneNode = node;

}

TextureCubeRenderComponent::TextureCubeRenderComponent(std::shared_ptr<Actor> owner, Scene* scene, float width, float height, float depth) : RenderComponent(std::move(owner), scene) 
{
	w = width;
	h = height;
	d = depth;
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

	std::vector<VertexNormalUV> vertices = GetVerts(w, h, d);
	// std::vector<VertexNormalUV> vertices = GetVerts(1, 1, 1);
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
	geometryDesc.vertexCount = vertices.size();
	geometryDesc.vertexData = vertices.data();
	geometryDesc.indexCount = 36;
	geometryDesc.indexData = cubeIdx;
	geometryDesc.topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

std::vector<VertexNormalUV> TextureCubeRenderComponent::GetVerts(float w, float h, float d)
{
	using namespace DirectX;

	std::vector<VertexNormalUV> verts;

	auto quad = [&](XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 n, float uScale, float vScale)
	{
		verts.push_back({ p0, n, {0, vScale} });
		verts.push_back({ p1, n, {uScale, vScale} });
		verts.push_back({ p2, n, {uScale, 0} });
		verts.push_back({ p3, n, {0, 0} });
	};

	quad({ w,-h,-d }, { w, h,-d }, { w, h, d }, { w,-h, d }, { 1,0,0 }, h, d);
	quad({ -w,-h, d }, { -w, h, d }, { -w, h,-d }, { -w,-h,-d }, { -1,0,0 }, h, d);
	quad({ -w, h,-d }, { w, h,-d }, { w, h, d }, { -w, h, d }, { 0,1,0 }, w, d);
	quad({ -w,-h, d }, { w,-h, d }, { w,-h,-d }, { -w,-h,-d }, { 0,-1,0 }, w, d);
	quad({ -w,-h, d }, { -w, h, d }, { w, h, d }, { w,-h, d }, { 0,0,1 }, h, w);
	quad({ w,-h,-d }, { w,h,-d }, { -w, h,-d }, { -w, -h,-d }, { 0,0,-1 }, h, w);

	return verts;
}

void LightComponent::VInit(DirectX::XMFLOAT3 colour, float intensity, std::array<float,3> attenuation)
{
	m_colour = colour;
	m_intensity = intensity;
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
		{
			light->SetColour(m_colour);
			light->SetIntensity(m_intensity);
		}
}

void LightComponent::SetColour(const DirectX::XMFLOAT3& colour)
{
	m_colour = colour;
}

void LightComponent::SetIntensity(float intensity) 
{
	m_intensity = intensity;
}



void WireframeCubeRenderComponent::VInit()
{
	using namespace DirectX;

	RenderComponent::VInit();

	const char* wireframeVS = R"(
	cbuffer Constants : register(b0)
	{
		matrix world;
		matrix viewProj;
	};

	struct VSInput
	{
		float3 pos : POSITION;
	};

	struct VSOutput
	{
		float4 pos : SV_Position;
	};

	VSOutput main(VSInput v)
	{
		VSOutput o;
		o.pos = mul(mul(float4(v.pos, 1.0), world), viewProj);
		return o;
	}
	)";

		const char* wireframePS = R"(
	float4 main(float4 pos : SV_Position) : SV_Target
	{
		return float4(0, 1, 0, 1);
	}
	)";

	ComPtr<ID3DBlob> vsBlob;
	auto vs = Graphics::CreateVertexShaderFromSource(wireframeVS, vsBlob);
	auto ps = Graphics::CreatePixelShaderFromSource(wireframePS);

	constexpr D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ComPtr<ID3D11InputLayout> inputLayout;
	Graphics::GetDevice()->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

	std::vector<Vertex> verts;
	std::vector<uint16_t> idx;
	BuildWireCube(verts, idx);

	ShaderMeshNode::GeometryDesc desc{};
	desc.vertexStride = sizeof(Vertex);
	desc.vertexCount = static_cast<uint32_t>(verts.size());
	desc.vertexData = verts.data();
	desc.indexCount = static_cast<uint32_t>(idx.size());
	desc.indexData = idx.data();
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	auto node = std::make_shared<ShaderMeshNode>(m_pOwner->GetId(), "wireframe", XMMatrixIdentity());
	m_scene->AddChildLast(m_pOwner->GetId(), node); // AddChildLast to draw over everything else

	node->SetGeometry(desc);
	node->VLoadResources(m_scene);
	node->SetShadersAndLayout(vs, ps, inputLayout);

	m_sceneNode = node;
}

void WireframeCubeRenderComponent::VUpdate(float deltaTime)
{
	using namespace DirectX;

	auto col = m_pOwner->GetComponent<AABBCollisionComponent>();
	if (col.expired())
		return;


	XMFLOAT3 p = m_pOwner->GetPosition(); // + col.lock()->GetCenter();
	XMFLOAT3 r = {0, 0, 0};
	XMFLOAT3 s = col.lock()->GetExtents();
	XMMATRIX world = XMMatrixScalingFromVector(XMLoadFloat3(&s)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&r)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&p));

	if(auto node = m_sceneNode.lock())
		node->VSetTransform(world);
}

void WireframeCubeRenderComponent::BuildWireCube(std::vector<Vertex>& verts, std::vector<uint16_t>& idx)
{
	DirectX::XMFLOAT3 corners[8] =
	{
		{-1, -1, -1},{1, -1, -1},{1, 1, -1},{-1, 1, -1},
		{-1, -1, 1},{1, -1, 1},{1, 1, 1},{-1, 1, 1}
	};

	constexpr uint16_t edges[24] =
	{
		0,1, 1,2, 2,3, 3,0,
		4,5, 5,6, 6,7, 7,4,
		0,4, 1,5, 2,6, 3,7
	};

	for (int i = 0; i < 8; i++)
		verts.push_back({corners[i]});

	for (int i = 0; i < 24; i++)
		idx.push_back({ edges[i] });
}
