#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "Graphics.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <chrono>

Graphics::Graphics(Application* app)
{
    s_appRef = app;
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

    uint32_t flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

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
	swapChainDescriptor.Width = s_appRef->GetWindowWidth();
    swapChainDescriptor.Height = s_appRef->GetWindowHeight();
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
        glfwGetWin32Window(s_appRef->GetWindow()),
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


    return true;
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

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(s_appRef->GetWindowWidth());
	viewport.Height = static_cast<float>(s_appRef->GetWindowHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// -------------------
	// Rendering stages

	s_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), clearColor);
	s_deviceContext->ClearDepthStencilView(m_depthStencil.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1, 0);

	// Rasterizer Stage
	s_deviceContext->RSSetViewports(1, &viewport);

	// Output Merger
	s_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), m_depthStencil.Get());
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

    // Depth stencil creation
    // ----------------------

    D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor = {};
    depthStencilDescriptor.DepthEnable = true;
	depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
    ComPtr<ID3D11DepthStencilState> depthStencilState;
    if (FAILED(s_device->CreateDepthStencilState(&depthStencilDescriptor, &depthStencilState)))
    {
        printf("D3D11: Failed to create depth stencil state\n");
        return false;
    }

    s_deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

    ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC depthTextureDescriptor = {};
    depthTextureDescriptor.Width = s_appRef->GetWindowWidth();
    depthTextureDescriptor.Height = s_appRef->GetWindowHeight();
    depthTextureDescriptor.MipLevels = 1;
    depthTextureDescriptor.ArraySize = 1;
    depthTextureDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
    depthTextureDescriptor.SampleDesc.Count = 1;
    depthTextureDescriptor.SampleDesc.Quality = 0;
    depthTextureDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    depthTextureDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
    if (FAILED(s_device->CreateTexture2D(&depthTextureDescriptor, nullptr, &pDepthStencil)))
    {
        printf("D3D11: Failed to create depth stencil texture\n");
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor = {};
    depthStencilViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDescriptor.Texture2D.MipSlice = 0;
    if (FAILED(s_device->CreateDepthStencilView(pDepthStencil.Get(), &depthStencilViewDescriptor, &m_depthStencil)))
    {
        printf("D3D11: Failed to create depth stencil view\n");
        return false;
    }

    return true;
}

void Graphics::DestroySwapchainResources()
{
    m_renderTarget.Reset();
    m_depthStencil.Reset();
}

bool Graphics::CompileShader(
    const std::wstring& fileName,
    const std::string& entryPoint,
    const std::string& profile,
    ComPtr<ID3DBlob>& shaderBlob)
{
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

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

Graphics::ComPtr<ID3D11VertexShader> Graphics::CreateVertexShaderFromSource(const char* src, ComPtr<ID3DBlob>& blobOut)
{
    ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompile(src, strlen(src), "VS", nullptr, nullptr, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &blobOut, &errorBlob);
    if (FAILED(hr))
    {
        printf("D3D11: Failed to compile vertex shader\n");
        return nullptr;
    }

    ComPtr<ID3D11VertexShader> vertexShader;
	GetDevice()->CreateVertexShader(blobOut->GetBufferPointer(), blobOut->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

    return vertexShader;
}

Graphics::ComPtr<ID3D11PixelShader> Graphics::CreatePixelShaderFromSource(const char* src)
{
    ComPtr<ID3DBlob> blob, errorBlob;
	HRESULT hr = D3DCompile(src, strlen(src), "PS", nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &blob, &errorBlob);
    if (FAILED(hr))
    {
        printf("D3D11: Failed to compile pixel shader\n");
        return nullptr;
    }

    ComPtr<ID3D11PixelShader> pixelShader;
	GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());

    return pixelShader;
}
