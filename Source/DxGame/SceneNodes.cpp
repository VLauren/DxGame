#include "SceneNodes.h"
#include "Graphics.h"

SceneNode::SceneNode(int actorId, std::string name, DirectX::XMMATRIX worldMatrix)
{
	m_actorId = actorId;
	m_name = name;
	m_worldMatrix = worldMatrix;
}

void SceneNode::VRenderChildren(Scene* pScene) 
{
	for(auto child : m_children)
		child->VPreRender(pScene);
	for(auto child : m_children)
		child->VRender(pScene);
	for(auto child : m_children)
		child->VPostRender(pScene);
}

bool SceneNode::VAddChild(std::shared_ptr<SceneNode> child) 
{
	m_children.emplace_back(child);
	return true;
}

bool SceneNode::VRemoveChild(int actorId) 
{
	return true;
}


	// -------------------
	// Geometry

	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
	};

	// cube vertices with colors
	static constexpr VertexPositionColor cubeVerts[8] = 
	{
		{ {-1,-1,-1}, {0.7f,0.2f,0.2f} }, { { 1,-1,-1}, {0.2f,0.7f,0.2f} },
		{ { 1, 1,-1}, {0.2f,0.2f,0.7f} }, { {-1, 1,-1}, {0.7f,0.7f,0.2f} },
		{ {-1,-1, 1}, {0.7f,0.2f,0.7f} }, { { 1,-1, 1}, {0.2f,0.7f,0.7f} },
		{ { 1, 1, 1}, {0.7f,0.7f,0.7f} }, { {-1, 1, 1}, {0.2f,0.2f,0.2f} }
	};

	static constexpr unsigned short cubeIdx[36] =
	{
		0,2,1, 2,0,3,
		4,6,7, 6,4,5,
		0,5,4, 5,0,1,
		2,7,6, 7,2,3,
		0,7,3, 7,0,4,
		1,6,5, 6,1,2
	};

void ShaderMeshNode::VLoadResources() 
{
	using namespace DirectX;

	// Vertex buffer creation
	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.ByteWidth = sizeof(cubeVerts);
	bufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA resourceData = {};
	resourceData.pSysMem = cubeVerts;
	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&bufferInfo,
		&resourceData,
		&m_vertexBuffer)))
	{
		printf("D3D11: Failed to create vertex buffer\n");
		return;
	}

	ConstantBuffer cvb;
	cvb.transform = XMMatrixTranspose(m_worldMatrix); // transpose because HLSL matrices are row major
	cvb.viewProj = XMMatrixTranspose(Graphics::GetViewProjMatrix());

	// Constant vertex buffer for transformations
	D3D11_BUFFER_DESC constVertBufferInfo = {};
	constVertBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	constVertBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	constVertBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	constVertBufferInfo.ByteWidth = sizeof(XMMATRIX) * 2;
	D3D11_SUBRESOURCE_DATA constVertResourceData = {};
	constVertResourceData.pSysMem = &cvb;
	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&constVertBufferInfo,
		&constVertResourceData,
		&m_vertexConstantBuffer)))
	{
		printf("D3D11: Failed to create constant vertex buffer\n");
		return;
	}

	// Index buffer creation
	D3D11_BUFFER_DESC indexBufferInfo = {};
	indexBufferInfo.ByteWidth = sizeof(cubeIdx);
	indexBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferInfo.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA indexResourceData = {};
	indexResourceData.pSysMem = cubeIdx;
	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&indexBufferInfo,
		&indexResourceData,
		&m_indexBuffer)))
	{
		printf("D3D11: Failed to create index buffer\n");
		return;
	}


	FrameCB cb{};
	cb.seed = m_frame++; // rolling seed

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
	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&constPixBufferInfo, &init, &m_pixelConstantBuffer);
	if (FAILED(hr))
	{
		printf("D3D11: Failed to create frame constant buffer\n");
	}

}

void ShaderMeshNode::SetShadersAndLayout(ComPtr<ID3D11VertexShader> vs, ComPtr<ID3D11PixelShader> ps, ComPtr<ID3D11InputLayout> il) 
{
	m_vertexShader = vs;
	m_pixelShader = ps;
	m_vertexLayout = il;
}

void ShaderMeshNode::VPreRender(Scene* pScene) 
{
}

void ShaderMeshNode::VRender(Scene* pScene)
{
	using namespace DirectX;

	auto device = Graphics::GetDevice();
	auto deviceContext = Graphics::GetDeviceContext();

	ConstantBuffer cvb;
	cvb.transform = XMMatrixTranspose(m_worldMatrix); // transpose because HLSL matrices are row major
	cvb.viewProj = XMMatrixTranspose(Graphics::GetViewProjMatrix());

	D3D11_MAPPED_SUBRESOURCE vertexMapResource;
	if (SUCCEEDED(Graphics::GetDeviceContext()->Map(m_vertexConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &vertexMapResource)))
	{
		*static_cast<ConstantBuffer*>(vertexMapResource.pData) = cvb;
		Graphics::GetDeviceContext()->Unmap(m_vertexConstantBuffer.Get(), 0);
	}

	// ------------------- 

	FrameCB cb{};
	cb.seed = m_frame++; // rolling seed

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
