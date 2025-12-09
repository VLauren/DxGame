#include "DxGameApp.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <ShlObj.h>

#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

using Position = DirectX::XMFLOAT3;
using Color = DirectX::XMFLOAT3;

struct VertexPositionColor
{
    Position position;
    Color color;
};

DxGameApp::DxGameApp(const std::string& title)
    : Application(title)
{
}

DxGameApp::~DxGameApp()
{
    Cleanup();
}

bool DxGameApp::Initialize()
{
    // Check if there already is an instance of the game running
    if (!IsOnlyInstance())
    {
        printf("There already is an instance of the game running.\n");
        return false;
    }

    // Check for enough disk space on the current disk
    if (!CheckStorage(10ULL * 1024 * 1024)) // 10 MB
    {
        printf("Not enough free space on the drive.\n");
    }

    // TODO add memory check

    // TODO initialize resource cache

    // Load strings
    LoadStrings("English");

    // printf("%ls", GetString(L"IDS_YES").c_str());

    // TODO Script manager and events system

    // Window creation
    if (!Application::Initialize())
        return false;

    // Set save game directory
	PWSTR path = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path)))
	{
		m_saveGameDirectory = std::wstring(path) + L"\\DXGame";
        CreateDirectory(m_saveGameDirectory.c_str(), nullptr);
		CoTaskMemFree(path);
	}
    else
    {
        printf("Unable to set save game directory\n");
        return false;
    }

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
        &m_device,
        nullptr,
        &m_deviceContext)))
    {
        printf("D3D11: Failed to create device and device context\n");
        return false;
    }

    // Swap chain creation
    // -------------------

    DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
    swapChainDescriptor.Width = GetWindowWidth();
    swapChainDescriptor.Height = GetWindowHeight();
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
        m_device.Get(),
        glfwGetWin32Window(GetWindow()),
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

    // -----------------

    // TODO Create game logic and game view

    // TODO Preload selected resources

    return true;
}

void DxGameApp::Cleanup()
{
    // TODO Delete game logic
    // TODO Delete event system and script manager
    // TODO Delete resource cache
}

// Loads shader and geometry data
bool DxGameApp::Load()
{
    ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
    m_vertexShader = CreateVertexShader(L"../Shaders/Main.vs.hlsl", vertexShaderBlob);
    if (m_vertexShader == nullptr)
        return false;

		m_pixelShader = CreatePixelShader(L"../Shaders/Main.ps.hlsl");
    if (m_pixelShader == nullptr)
        return false;

    constexpr D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(VertexPositionColor, position),
            D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            offsetof(VertexPositionColor, color),
            D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
    };

    if (FAILED(m_device->CreateInputLayout(
            vertexInputLayoutInfo,
            _countof(vertexInputLayoutInfo),
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            &m_vertexLayout)))
    {
        printf("D3D11: Failed to create default vertex input layout\n");
        return false;
    }

    constexpr VertexPositionColor vertices[] = {
        {   Position{ 0.0f, 0.6f, 0.0f }, Color{ 0.6f, 0.3f, 0.3f } },
        {  Position{ 0.4f, -0.4f, 0.0f }, Color{ 0.6f, 0.6f, 0.3f } },
        { Position{ -0.4f, -0.4f, 0.0f }, Color{ 0.3f, 0.3f, 0.6f } },
    };

    D3D11_BUFFER_DESC bufferInfo = {};
    bufferInfo.ByteWidth = sizeof(vertices);
    bufferInfo.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
    bufferInfo.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = vertices;
    if (FAILED(m_device->CreateBuffer(
        &bufferInfo,
        &resourceData,
        &m_triangleVertices)))
    {
        printf("D3D11: Failed to create triangle vertex buffer\n");
        return false;
    }
    return true;
}

bool DxGameApp::CreateSwapchainResources()
{
    ComPtr<ID3D11Texture2D> backBuffer = nullptr;

    if (FAILED(m_swapChain->GetBuffer(
        0,
        IID_PPV_ARGS(&backBuffer))))
    {
        printf("D3D11: Failed to get back buffer from the swap chain\n");
        return false;
    }

    if (FAILED(m_device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &m_renderTarget)))
    {
        printf("D3D11: Failed to create render target view from back buffer\n");
        return false;
    }

    return true;
}

void DxGameApp::DestroySwapchainResources()
{
    m_renderTarget.Reset();
}

void DxGameApp::OnResize(int32_t width, int32_t height)
{
    Application::OnResize(width, height);
    m_deviceContext->Flush();

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

bool DxGameApp::CompileShader(
    const std::wstring& fileName,
    const std::string& entryPoint,
    const std::string& profile,
    ComPtr<ID3DBlob>& shaderBlob) const
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

DxGameApp::ComPtr<ID3D11VertexShader> DxGameApp::CreateVertexShader(
    const std::wstring& fileName,
    ComPtr<ID3DBlob>& vertexShaderBlob) const
{
    if (!CompileShader(fileName, "main", "vs_5_0", vertexShaderBlob))
        return nullptr;

    ComPtr<ID3D11VertexShader> vertexShader;
    if (FAILED(m_device->CreateVertexShader(
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

DxGameApp::ComPtr<ID3D11PixelShader> DxGameApp::CreatePixelShader(const std::wstring& fileName) const
{
    ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
    if (!CompileShader(fileName, "main", "ps_5_0", pixelShaderBlob))
    {
        return nullptr;
    }

    ComPtr<ID3D11PixelShader> pixelShader;
    if (FAILED(m_device->CreatePixelShader(
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

bool DxGameApp::IsOnlyInstance()
{
    HANDLE handle = CreateMutex(nullptr, true, L"DxGame");
    if (GetLastError() != ERROR_SUCCESS)
    {
        HWND existingWnd = FindWindow(nullptr, L"DxGame");
        if (existingWnd)
        {
            ShowWindow(existingWnd, SW_SHOWNORMAL);
            SetFocus(existingWnd);
            SetForegroundWindow(existingWnd);
            SetActiveWindow(existingWnd);
			return false;
        }
    }

    return true;
}

bool DxGameApp::CheckStorage(std::uintmax_t diskSpaceNeeded)
{
    // Find disk space available
    namespace fs = std::filesystem;
    const fs::path root = fs::path(fs::current_path()).root_path();
    const fs::space_info si = fs::space(root);

    // Check if it's enough
    return si.available >= diskSpaceNeeded;
}

bool DxGameApp::CheckMemory(DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
    MEMORYSTATUSEX status{ sizeof(status) };
    GlobalMemoryStatusEx(&status);

    // Memory availability
    if (status.ullTotalPhys < physicalRAMNeeded)
        return false;
    if (status.ullAvailVirtual < virtualRAMNeeded)
        return false;

    // Check if memory is available in one block
    char* buff = DXG_NEW char[virtualRAMNeeded];
    if (buff)
        delete[] buff;
    else
        return false;

    return true;
}

bool DxGameApp::LoadStrings(std::string language)
{
    std::string langFile = R"(..\..\Assets\Strings\)" + language + ".xml";

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(langFile.c_str()) != tinyxml2::XMLError::XML_SUCCESS)
    {
        printf("Strings are missing");
        return false;
    }

    const tinyxml2::XMLElement* root = doc.FirstChildElement("strings");
    if (!root)
    {
        printf("XML file format error");
        return false;
    }

    // root->FirstChildElement("string")
    for (const tinyxml2::XMLElement* pElem = root->FirstChildElement("string"); pElem != nullptr; pElem = pElem->NextSiblingElement())
    {
        const char* pKey = pElem->Attribute("id");
        const char* pText = pElem->Attribute("value");
        if (pKey && pText)
        {
            // lambda helper to convert to wide string
            auto Utf8ToWide = [](std::string_view u8) -> std::wstring
                {
                    if (u8.empty())
                        return {};
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, u8.data(), (int)u8.size(), nullptr, 0);
                    std::wstring ws(wlen, L'\0');
                    MultiByteToWideChar(CP_UTF8, 0, u8.data(), (int)u8.size(), ws.data(), wlen);

                    return ws;
                };

            std::wstring key = Utf8ToWide(pKey);
            std::wstring text = Utf8ToWide(pText);
            m_textResource[key] = text;
        }
    }

    return true;
}

std::wstring DxGameApp::GetString(std::wstring sID)
{
    auto localizedString = m_textResource.find(sID);
    if (localizedString == m_textResource.end())
    {
        printf("Missing localisation key: %ls\n", sID.c_str());
        return sID;
    }

    return localizedString->second;
}

void DxGameApp::Update()
{
}

void DxGameApp::Render()
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(GetWindowWidth());
    viewport.Height = static_cast<float>(GetWindowHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    constexpr float clearColor[] = { 0.1f, 0.12f, 0.1f, 0.1f };
    constexpr UINT vertexStride = sizeof(VertexPositionColor);
    constexpr UINT vertexOffset = 0;

    m_deviceContext->ClearRenderTargetView(m_renderTarget.Get(), clearColor);

    // Input Assembler
    m_deviceContext->IASetInputLayout(m_vertexLayout.Get());
    m_deviceContext->IASetVertexBuffers(0, 1, m_triangleVertices.GetAddressOf(), &vertexStride, &vertexOffset);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Vertex Shader
    m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

    // Rasterizer Stage
    m_deviceContext->RSSetViewports(1, &viewport);

    // Pixel Shader
    m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // Output Merger
    m_deviceContext->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);

    m_deviceContext->Draw(3, 0);
    m_swapChain->Present(1, 0);
}

