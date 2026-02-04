#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "DxGameApp.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_dx11.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <ShlObj.h>

#include <iostream>
#include "Actors/RenderComponents.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

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

    // Graphics object creation
    m_gfx = std::make_unique<Graphics>(this);
    if (!m_gfx->Initialize())
        return false;

    m_scene = std::make_unique<Scene>();

    // Setup Dear Imgui Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(m_window, true);
    ImGui_ImplDX11_Init(Graphics::GetDevice().Get(), Graphics::GetDeviceContext().Get());
    // -----------------

    // WIP Create game logic and game view
    m_game = std::make_unique<Game>(m_scene.get(), m_window);
    m_game->Init();
    return true;

    // TODO Preload selected resources

    return true;
}

void DxGameApp::Cleanup()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplDX11_Shutdown();
    return;

    // Delete game logic
    m_game->Destroy();

    // TODO Delete event system and script manager
    // TODO Delete resource cache
}

// Loads shader and geometry data
bool DxGameApp::Load()
{
    if (!m_gfx->Load())
        return false;

    return true;
}


void DxGameApp::OnResize(int32_t width, int32_t height)
{
    Application::OnResize(width, height);

    m_gfx->OnResize(width, height);
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

void DxGameApp::Update(float deltaTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    m_game->Update(deltaTime);
}

void DxGameApp::Render()
{
    m_gfx->Render();
    m_scene->OnRender();

	bool show_demo_window = false;

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_gfx->Present();
}

