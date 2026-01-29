#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include <filesystem>
#include <crtdbg.h>

#include "../Application.h"

class Graphics
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    // inline static DirectX::XMMATRIX s_viewProj = DirectX::XMMatrixIdentity();
    inline static ComPtr<ID3D11DeviceContext> s_deviceContext = nullptr;
    inline static ComPtr<ID3D11Device> s_device = nullptr;

public:
    Graphics(Application* app);
	~Graphics();

    // static const DirectX::XMMATRIX& GetViewProjMatrix() { return s_viewProj; }
    static const ComPtr<ID3D11Device>& GetDevice() { return s_device; }
    static const ComPtr<ID3D11DeviceContext>& GetDeviceContext() { return s_deviceContext; }
    static const Application* GetApplication() { return s_appRef; }
    inline static bool s_showWireframe;
	
	bool Initialize();
	void Render();
    void Present();
    bool Load();
    void OnResize(int32_t width, int32_t height);

    bool CreateSwapchainResources();
    void DestroySwapchainResources();

    static bool CompileShader(
        const std::wstring& fileName,
        const std::string& entryPoint,
        const std::string& profile,
        ComPtr<ID3DBlob>& shaderBlob);

	[[nodiscard]] static ComPtr<ID3D11VertexShader> CreateVertexShader(const std::wstring& fileName, ComPtr<ID3DBlob>& vertexShaderBlob);
    [[nodiscard]] static ComPtr<ID3D11PixelShader> CreatePixelShader(const std::wstring& fileName);

	[[nodiscard]] static ComPtr<ID3D11VertexShader> CreateVertexShaderFromSource(const char* src, ComPtr<ID3DBlob>& vertexShaderBlob);
    [[nodiscard]] static ComPtr<ID3D11PixelShader> CreatePixelShaderFromSource(const char* src);

    inline static Application* s_appRef;

    ComPtr<ID3D11Device> m_device = nullptr;
    ComPtr<IDXGIFactory2> m_dxgiFactory = nullptr;
    ComPtr<IDXGISwapChain1> m_swapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> m_renderTarget = nullptr;
    ComPtr<ID3D11DepthStencilView> m_depthStencil = nullptr;
};

