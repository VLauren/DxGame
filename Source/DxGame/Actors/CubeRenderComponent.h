#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include "ActorComponent.h"
#include "../SceneNodes.h"

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class CubeRenderComponent : public ActorComponent
{
public:
	explicit CubeRenderComponent(std::shared_ptr<Actor> owner, class Scene* scene) : ActorComponent(std::move(owner)), m_scene(scene) {}

	void VInit();

	void VUpdate(float deltaTime) override;

private:
	uint32_t m_frame = 0;

    std::weak_ptr<ShaderMeshNode> m_sceneNode;

    class Scene* m_scene;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
};
