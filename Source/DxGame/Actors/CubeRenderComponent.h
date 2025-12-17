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

// TODO
// [component de cubo de dibuja]
// version minima, estructura cutre
// - desde Application.cpp, Render() lo que hacer es, gfx Render, m_game por cada actor, por cada render component Render(), gfx Present()
// - mover present a gfx Present, ver que todo va bien y se llama cube render
// - mover a cube render: geometria, todo lo que depende de geometría manteniendo el orden
// - ver si pinta bien
// - ver si puedo mover mas
// - calcular world transform con datos de actor

