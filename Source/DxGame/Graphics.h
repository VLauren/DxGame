#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include <filesystem>
#include <crtdbg.h>

#include "Application.h"

class Graphics
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    Graphics(Application* app);
	~Graphics();

	
	bool Initialize();
	void Render();
    bool Load();
    void OnResize(int32_t width, int32_t height);

    bool CreateSwapchainResources();
    void DestroySwapchainResources();

    bool CompileShader(
        const std::wstring& fileName,
        const std::string& entryPoint,
        const std::string& profile,
        ComPtr<ID3DBlob>& shaderBlob) const;

    [[nodiscard]] ComPtr<ID3D11VertexShader> CreateVertexShader(
        const std::wstring& fileName,
        ComPtr<ID3DBlob>& vertexShaderBlob) const;

    [[nodiscard]] ComPtr<ID3D11PixelShader> CreatePixelShader(const std::wstring& fileName) const;

    Application* m_appRef;

    ComPtr<ID3D11Device> m_device = nullptr;
    ComPtr<ID3D11DeviceContext> m_deviceContext = nullptr;
    ComPtr<IDXGIFactory2> m_dxgiFactory = nullptr;
    ComPtr<IDXGISwapChain1> m_swapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> m_renderTarget = nullptr;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;

    ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
    ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;
	uint32_t m_frame = 0; // rolling seed
};

