#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include "ActorComponent.h"
#include "../Graphics/SceneNodes.h"

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

    std::weak_ptr<SceneNode> m_sceneNode;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;
};

class CubeRenderComponent : public RenderComponent
{
public:
	explicit CubeRenderComponent(std::shared_ptr<Actor> owner, Scene* scene) : RenderComponent(std::move(owner), scene) {}

	void VInit();
};

class TextureCubeRenderComponent : public RenderComponent
{
public:
    explicit TextureCubeRenderComponent(std::shared_ptr<Actor> owner, Scene* scene) : RenderComponent(std::move(owner), scene) {}

    void VInit();

    ComPtr<ID3D11ShaderResourceView> m_diffuseSRV = nullptr;
    ComPtr<ID3D11SamplerState> m_sampler = nullptr;
};

class LightComponent : public RenderComponent
{
public:
    explicit LightComponent(std::shared_ptr<Actor> owner, Scene* scene) : RenderComponent(std::move(owner), scene) {}

    void VInit(DirectX::XMFLOAT3 colour, float intensity, std::array<float,3> attenuation);
    void VUpdate(float deltaTime) override;
    void SetColour(const DirectX::XMFLOAT3& colour);
    DirectX::XMFLOAT3 GetColour() const { return m_colour; }
    void SetIntensity(float intensity);
    float GetIntensity() const { return m_intensity; }

private:
	DirectX::XMFLOAT3 m_colour = DirectX::XMFLOAT3(1, 1, 1);
    float m_intensity;
};
