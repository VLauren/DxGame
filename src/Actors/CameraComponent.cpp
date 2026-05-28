// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include "CameraComponent.h"

// #include <DirectXMath.h>
// #include <d3d11.h>
#include "../Graphics/Scene.h"
#include "../Graphics/Graphics.h"

void CameraComponent::VInit()
{
	auto node = std::make_shared<CameraNode>(m_pOwner->GetId(), std::string("Camera node"), DirectX::XMMatrixIdentity());
	m_scene->AddChild(m_pOwner->GetId(), node);

	m_sceneNode = node;
}

void CameraComponent::VUpdate(float deltaTime) 
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
