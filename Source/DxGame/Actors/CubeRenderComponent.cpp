#include "CubeRenderComponent.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include "../Graphics.h"

using Position = DirectX::XMFLOAT3;
using Color = DirectX::XMFLOAT3;

struct VertexPositionColor
{
    Position position;
    Color color;
};

void CubeRenderComponent::VInit()
{
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

	// Constant pixel buffer for the dynamic shader
	struct alignas(16) FrameCB { uint32_t seed; uint32_t pad[3]; }; // 16 bytes
	D3D11_BUFFER_DESC constPixBufferInfo = {};
	constPixBufferInfo.ByteWidth = sizeof(FrameCB);
	constPixBufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	constPixBufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constPixBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA init = {};
	FrameCB initial = {};
	init.pSysMem = &initial;
	HRESULT hr = device->CreateBuffer(&constPixBufferInfo, &init, &m_pixelConstantBuffer);
	if (FAILED(hr))
	{
		printf("D3D11: Failed to create frame constant buffer\n");
	}
}

void CubeRenderComponent::VUpdate(float deltaTime)
{
}

void CubeRenderComponent::Render()
{
	using namespace DirectX;

	auto device = Graphics::GetDevice();
	auto deviceContext = Graphics::GetDeviceContext();
	
	XMFLOAT3 p = m_pOwner->GetPosition();
	XMFLOAT3 r = m_pOwner->GetRotation();
	XMFLOAT3 s = m_pOwner->GetScale();
	XMMATRIX world = XMMatrixScalingFromVector(XMLoadFloat3(&s)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&r)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&p));
	// XMMATRIX world = XMMatrixIdentity();

	// ------------------- 
	// Change over time

	struct FrameCB { uint32_t seed; uint32_t pad[3]; };
	FrameCB cb{};
	cb.seed = m_frame++; // rolling seed

	// -------------------
	// Geometry

	// cube vertices with colors
	constexpr VertexPositionColor cubeVerts[8] = 
	{
		{ {-1,-1,-1}, {0.7f,0.2f,0.2f} }, { { 1,-1,-1}, {0.2f,0.7f,0.2f} },
		{ { 1, 1,-1}, {0.2f,0.2f,0.7f} }, { {-1, 1,-1}, {0.7f,0.7f,0.2f} },
		{ {-1,-1, 1}, {0.7f,0.2f,0.7f} }, { { 1,-1, 1}, {0.2f,0.7f,0.7f} },
		{ { 1, 1, 1}, {0.7f,0.7f,0.7f} }, { {-1, 1, 1}, {0.2f,0.2f,0.2f} }
	};
	constexpr unsigned short cubeIdx[36] =
	{
		0,2,1, 2,0,3,
		4,6,7, 6,4,5,
		0,5,4, 5,0,1,
		2,7,6, 7,2,3,
		0,7,3, 7,0,4,
		1,6,5, 6,1,2
	};

	// -------------------

	// Vertex buffer creation
	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.ByteWidth = sizeof(cubeVerts);
	bufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA resourceData = {};
	resourceData.pSysMem = cubeVerts;
	if (FAILED(device->CreateBuffer(
		&bufferInfo,
		&resourceData,
		&m_vertexBuffer)))
	{
		printf("D3D11: Failed to create vertex buffer\n");
		return;
	}

	// Constant vertex buffer for transformations
	struct ConstantBuffer
	{
		XMMATRIX transform;
		XMMATRIX viewProj;
	};

	ConstantBuffer cvb;
	cvb.transform = XMMatrixTranspose(world); // transpose because HLSL matrices are row major
	cvb.viewProj = XMMatrixTranspose(Graphics::GetViewProjMatrix());

	D3D11_BUFFER_DESC constVertBufferInfo = {};
	constVertBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	constVertBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	constVertBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	constVertBufferInfo.ByteWidth = sizeof(cvb);
	D3D11_SUBRESOURCE_DATA constVertResourceData = {};
	constVertResourceData.pSysMem = &cvb;
	if (FAILED(device->CreateBuffer(
		&constVertBufferInfo,
		&constVertResourceData,
		&m_vertexConstantBuffer)))
	{
		printf("D3D11: Failed to create constant vertex buffer\n");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE vertexMapResource;
	if (SUCCEEDED(deviceContext->Map(m_vertexConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &vertexMapResource)))
	{
		*static_cast<ConstantBuffer*>(vertexMapResource.pData) = cvb;
		deviceContext->Unmap(m_vertexConstantBuffer.Get(), 0);
	}

	// Index buffer creation
	D3D11_BUFFER_DESC indexBufferInfo = {};
	indexBufferInfo.ByteWidth = sizeof(cubeIdx);
	indexBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferInfo.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA indexResourceData = {};
	indexResourceData.pSysMem = cubeIdx;
	if (FAILED(device->CreateBuffer(
		&indexBufferInfo,
		&indexResourceData,
		&m_indexBuffer)))
	{
		printf("D3D11: Failed to create index buffer\n");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pixelConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		*static_cast<FrameCB*>(mapped.pData) = cb;
		deviceContext->Unmap(m_pixelConstantBuffer.Get(), 0);
	}

	constexpr UINT vertexStride = sizeof(VertexPositionColor);
	constexpr UINT vertexOffset = 0;

	// Input Assembler
	deviceContext->IASetInputLayout(m_vertexLayout.Get());
	deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset);
	deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Vertex Shader
	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	// Pixel Shader
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	deviceContext->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf());

	deviceContext->DrawIndexed(std::size(cubeIdx), 0, 0);
}
