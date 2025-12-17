#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include "ActorComponent.h"

class CubeRenderComponent : public ActorComponent
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	explicit CubeRenderComponent(std::shared_ptr<Actor> owner) : ActorComponent(std::move(owner)) {}

	void VInit();

	void VUpdate(float deltaTime) override;

	void Render();

	uint32_t m_frame = 0;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;

    ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
    ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;
};
