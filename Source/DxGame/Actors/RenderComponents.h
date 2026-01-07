#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include "ActorComponent.h"
#include "../SceneNodes.h"

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class Scene;

class RenderComponent : public ActorComponent
{
public:
    explicit RenderComponent(std::shared_ptr<Actor> owner, Scene* scene) : ActorComponent(std::move(owner)), m_scene(scene) {}

	void VInit();

	void VUpdate(float deltaTime) override;

protected:

    Scene* m_scene;

    std::weak_ptr<ShaderMeshNode> m_sceneNode;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
};

class CubeRenderComponent : public RenderComponent
{
public:
	explicit CubeRenderComponent(std::shared_ptr<Actor> owner, Scene* scene) : RenderComponent(std::move(owner), scene) {}

	void VInit();

private:
	uint32_t m_frame = 0;
};
