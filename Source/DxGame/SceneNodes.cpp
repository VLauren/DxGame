#include "Scene.h"
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

// ===========================
//		Shader Mesh Node
// ===========================

void ShaderMeshNode::SetGeometry(const GeometryDesc& desc) 
{
	m_geometryDesc = desc;
}

void ShaderMeshNode::VLoadResources(Scene* pScene)
{
	using namespace DirectX;

	// Vertex buffer
	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.ByteWidth = m_geometryDesc.vertexStride * m_geometryDesc.vertexCount;
	bufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA resourceData = {};
	resourceData.pSysMem = m_geometryDesc.vertexData;
	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&bufferInfo,
		&resourceData,
		&m_vertexBuffer)))
	{
		printf("D3D11: Failed to create vertex buffer\n");
		return;
	}

	// Index buffer creation
	D3D11_BUFFER_DESC indexBufferInfo = {};
	indexBufferInfo.ByteWidth = sizeof(uint16_t) * m_geometryDesc.indexCount;
	indexBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferInfo.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA indexResourceData = {};
	indexResourceData.pSysMem = m_geometryDesc.indexData;
	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&indexBufferInfo,
		&indexResourceData,
		&m_indexBuffer)))
	{
		printf("D3D11: Failed to create index buffer\n");
		return;
	}

	// Constant vertex buffer for transformations
	ConstantBuffer cvb;
	cvb.transform = XMMatrixTranspose(m_worldMatrix); // transpose because HLSL matrices are row major
	cvb.viewProj = XMMatrixTranspose(pScene->GetViewProjMatrix());
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

	// per object constants
	ConstantBuffer cvb;
	cvb.transform = XMMatrixTranspose(m_worldMatrix); // transpose because HLSL matrices are row major
	cvb.viewProj = XMMatrixTranspose(pScene->GetViewProjMatrix());
	D3D11_MAPPED_SUBRESOURCE vertexMapResource;
	if (SUCCEEDED(Graphics::GetDeviceContext()->Map(m_vertexConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &vertexMapResource)))
	{
		*static_cast<ConstantBuffer*>(vertexMapResource.pData) = cvb;
		Graphics::GetDeviceContext()->Unmap(m_vertexConstantBuffer.Get(), 0);
	}

	// rolling seed
	FrameCB cb{};
	cb.seed = m_frame++; // rolling seed
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(deviceContext->Map(m_pixelConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		*static_cast<FrameCB*>(mapped.pData) = cb;
		deviceContext->Unmap(m_pixelConstantBuffer.Get(), 0);
	}

	// Input Assembler
	UINT vertexStride = m_geometryDesc.vertexStride;
	constexpr UINT vertexOffset = 0;
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

	// Draw
	deviceContext->DrawIndexed(m_geometryDesc.indexCount, 0, 0);
}


// ===========================
//		Camera Node
// ===========================

void CameraNode::VPreRender(Scene* pScene)
{
}

void CameraNode::VRender(Scene* pScene)
{
	using namespace DirectX;

	
	float aspect = (float) Graphics::GetApplication()->GetWindowWidth() / Graphics::GetApplication()->GetWindowHeight();
	m_fov = XM_PI / 3.f; // vertical fov: 60 deg
	m_nearZ = 0.1f;
	m_farZ = 100.f;

	// XMMATRIX view = XMMatrixLookAtLH(
		// XMVectorSet(0, 2, -7, 0),
		// XMVectorSet(0, 0, 0, 0),
		// XMVectorSet(0, 1, 0, 0));

	XMMATRIX view = XMMatrixInverse(nullptr, m_worldMatrix);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(m_fov, aspect, m_nearZ, m_farZ);

	pScene->SetViewProjMatrix(view * proj);
}

void CameraNode::VRenderChildren(Scene* pScene)
{
}

void CameraNode::VPostRender(Scene* pScene)
{
}

// ===========================
//		Light Node
// ===========================

void LightNode::VLoadResources(Scene* pScene)
{
	using namespace DirectX;

	struct LightConstantBuf
	{
		XMFLOAT3 Pos;
		float _pad0; // 12 + 4 = 16
		XMFLOAT3 Color;
		float _pad1; // 12 + 4 = 16
		float Intensity;
		float AttConst;
		float AttLin;
		float AttQuad;
	};

	static_assert(sizeof(LightConstantBuf) % 16 == 0);

	LightConstantBuf lightBuf = {};
	lightBuf.Pos = XMFLOAT3(-2, 2, -2);
	lightBuf.Color = XMFLOAT3(1, 1, 1);
	lightBuf.Intensity = 1.0f;
	lightBuf.AttConst = 1.0;
	lightBuf.AttLin = 0.09f;
	lightBuf.AttQuad = 0.032f;

	D3D11_BUFFER_DESC constVertBufferInfo = {};
	constVertBufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	constVertBufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	constVertBufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	constVertBufferInfo.ByteWidth = sizeof(LightConstantBuf);

	D3D11_SUBRESOURCE_DATA constVertResourceData = { &lightBuf };

	if (FAILED(Graphics::GetDevice()->CreateBuffer(
		&constVertBufferInfo,
		&constVertResourceData,
		&m_constantBuffer)))
	{
		// HRESULT reason = Graphics::GetDevice()->GetDeviceRemovedReason();
		// printf("D3D11: Failed to create light constant vertex buffer: 0x%08X device removed 0x%08X\n", );
		printf("D3D11: Failed to create light constant vertex buffer\n");
		return;
	}
}

void LightNode::VRender(Scene* pScene)
{
	using namespace DirectX;

	struct LightConstantBuf
	{
		XMFLOAT3 Pos;
		float _pad0; // 12 + 4 = 16
		XMFLOAT3 Color;
		float _pad1; // 12 + 4 = 16
		float Intensity;
		float AttConst;
		float AttLin;
		float AttQuad;
	};

	LightConstantBuf lightBuf = {};
	lightBuf.Pos = XMFLOAT3(-2, 2, -2);
	lightBuf.Color = XMFLOAT3(1, 1, 1);
	lightBuf.Intensity = 1.0f;
	lightBuf.AttConst = 1.0;
	lightBuf.AttLin = 0.09f;
	lightBuf.AttQuad = 0.032f;

	D3D11_MAPPED_SUBRESOURCE mr;
	if (SUCCEEDED(Graphics::GetDeviceContext()->Map(
		m_constantBuffer.Get(),
		0,
		D3D11_MAP::D3D11_MAP_WRITE_DISCARD,
		0,
		&mr)))
	{
		*static_cast<LightConstantBuf*>(mr.pData) = lightBuf;
		Graphics::GetDeviceContext()->Unmap(m_constantBuffer.Get(), 0);
	}

	Graphics::GetDeviceContext()->PSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
}
