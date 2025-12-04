#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include <filesystem>
#include <crtdbg.h>

// TODO debug new
#define DXG_NEW new

#include "Application.h"

class DxGameApp : public Application
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    DxGameApp(const std::string& title);
    ~DxGameApp() override;

protected:
    bool Initialize() override;
    bool Load() override;
    void OnResize(int32_t width, int32_t height) override;
    void Render() override;
    void Update() override;

private:
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

    ComPtr<ID3D11Device> m_device = nullptr;
    ComPtr<ID3D11DeviceContext> m_deviceContext = nullptr;
    ComPtr<IDXGIFactory2> m_dxgiFactory = nullptr;
    ComPtr<IDXGISwapChain1> m_swapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> m_renderTarget = nullptr;

    ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
    ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;

    ComPtr<ID3D11Buffer> m_triangleVertices = nullptr;
    ComPtr<ID3D11InputLayout> m_vertexLayout = nullptr;


    bool IsOnlyInstance();
    bool CheckStorage(std::uintmax_t diskSpaceNeeded);
    bool CheckMemory(DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
};

