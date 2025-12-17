
#include "Graphics.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <chrono>

using Position = DirectX::XMFLOAT3;
using Color = DirectX::XMFLOAT3;

struct VertexPositionColor
{
    Position position;
    Color color;
};

Graphics::Graphics(Application* app)
{
    m_appRef = app;
}

bool Graphics::Initialize() 
{
    // Device creation
    // ---------------

    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory))))
    {
        printf("DXGI: Unable to create DXGIFactory\n");
        return false;
    }

    constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

    if (FAILED(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        &deviceFeatureLevel,
        1,
        D3D11_SDK_VERSION,
        &s_device,
        nullptr,
        &s_deviceContext)))
    {
        printf("D3D11: Failed to create device and device context\n");
        return false;
    }

    // Swap chain creation
    // -------------------

    DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
	swapChainDescriptor.Width = m_appRef->GetWindowWidth();
    swapChainDescriptor.Height = m_appRef->GetWindowHeight();
    swapChainDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.SampleDesc.Quality = 0;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescriptor.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDescriptor.Flags = {};

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDescriptor = {};
    swapChainFullScreenDescriptor.Windowed = true;

    if (FAILED(m_dxgiFactory->CreateSwapChainForHwnd(
        s_device.Get(),
        glfwGetWin32Window(m_appRef->GetWindow()),
        &swapChainDescriptor,
        &swapChainFullScreenDescriptor,
        nullptr,
        &m_swapChain)))
    {
        printf("DXGI: Failed to create swapchain\n");
        return false;
    }

    // Create back buffer and render target
    if (!CreateSwapchainResources())
    {
        return false;
    }
}

Graphics::~Graphics()
{
}

bool Graphics::Load()
{
	return true;
}

void Graphics::Render()
{
	using namespace DirectX;

	const double oscillation = sin(m_appRef->GetTotalGameTime()) / 2.0f + 0.5f;
	float angle = DirectX::XM_PI * m_appRef->GetTotalGameTime() / 3;

	// -------------------
	// Hardcoded camera

	float aspect = (float)m_appRef->GetWindowWidth() / m_appRef->GetWindowHeight();
	float fov = XM_PI / 3.f; // vertical fov: 60 deg
	float nearZ = 0.1f;
	float farZ = 100.f;

	//XMMATRIX view = XMMatrixLookToLH(
	XMMATRIX view = XMMatrixLookAtLH(
		XMVectorSet(0, 2, -5, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));

	XMMATRIX proj = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

    s_viewProj = view * proj;

	// rotate around Y
	// float time = m_appRef->GetTotalGameTime();

	// -------------------

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(m_appRef->GetWindowWidth());
	viewport.Height = static_cast<float>(m_appRef->GetWindowHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// constexpr float clearColor[] = { 0.1f, 0.12f, 0.1f, 0.1f };
	constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// const float clearColor[] = { 0.2f + oscillation * 0.1f, 0.2f + oscillation * 0.4f, 0.2f, 1.0f }; // Oscillating colour

	// -------------------
	// Rendering stages

	s_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), clearColor);


	// Rasterizer Stage
	s_deviceContext->RSSetViewports(1, &viewport);


	// Output Merger
	s_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);

}

void Graphics::Present()
{
	m_swapChain->Present(1, 0);
}

void Graphics::OnResize(int32_t width, int32_t height)
{
    s_deviceContext->Flush();

    DestroySwapchainResources();

    if (FAILED(m_swapChain->ResizeBuffers(
            0,
            width,
            height,
            DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            0)))
    {
        printf("D3D11: Failed to recreate SwapChain buffers\n");
        return;
    }

    CreateSwapchainResources();

}

bool Graphics::CreateSwapchainResources()
{
    ComPtr<ID3D11Texture2D> backBuffer = nullptr;

    if (FAILED(m_swapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&backBuffer))))
    {
        printf("D3D11: Failed to get back buffer from the swap chain\n");
        return false;
    }

    if (FAILED(s_device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &m_renderTarget)))
    {
        printf("D3D11: Failed to create render target view from back buffer\n");
        return false;
    }

    return true;
}

void Graphics::DestroySwapchainResources()
{
    m_renderTarget.Reset();
}

bool Graphics::CompileShader(
    const std::wstring& fileName,
    const std::string& entryPoint,
    const std::string& profile,
    ComPtr<ID3DBlob>& shaderBlob)
{
    constexpr UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    ComPtr<ID3DBlob> tempShaderBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    if (FAILED(D3DCompileFromFile(
        fileName.data(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.data(),
        profile.data(),
        compileFlags,
        0,
        &tempShaderBlob,
        &errorBlob)))
    {
        printf("D3D11: Failed to read shader from file\n");
        if (errorBlob != nullptr)
        {
            printf("D3D11: with message: %s\n", static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        return false;
    }

    shaderBlob = std::move(tempShaderBlob);
    return true;
}

Graphics::ComPtr<ID3D11VertexShader> Graphics::CreateVertexShader(
    const std::wstring& fileName,
    ComPtr<ID3DBlob>& vertexShaderBlob)
{
    if (!CompileShader(fileName, "main", "vs_5_0", vertexShaderBlob))
        return nullptr;

    ComPtr<ID3D11VertexShader> vertexShader;
    if (FAILED(s_device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader)))
    {
        printf("D3D11: Failed to compile vertex shader\n");
        return nullptr;
    }

    return vertexShader;
}

Graphics::ComPtr<ID3D11PixelShader> Graphics::CreatePixelShader(const std::wstring& fileName)
{
    ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
    if (!CompileShader(fileName, "main", "ps_5_0", pixelShaderBlob))
    {
        return nullptr;
    }

    ComPtr<ID3D11PixelShader> pixelShader;
    if (FAILED(s_device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        &pixelShader)))
    {
        printf("D3D11: Failed to compile pixel shader\n");
        return nullptr;
    }

    return pixelShader;
}
