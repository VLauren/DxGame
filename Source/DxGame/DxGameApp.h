#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>

#include <filesystem>
#include <crtdbg.h>

// TODO debug new
#define DXG_NEW new

#include "Application.h"
#include "Graphics.h"
#include <tinyxml2.h>

class DxGameApp : public Application
{
    // template <typename T>
    // using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    DxGameApp(const std::string& title);
    ~DxGameApp() override;

protected:
    void Cleanup() override;
    bool Initialize() override;
    bool Load() override;
    void OnResize(int32_t width, int32_t height) override;
    void Render() override;
    void Update(float deltaTime) override;

private:
    std::unique_ptr<Graphics> m_gfx;

    bool IsOnlyInstance();
    bool CheckStorage(std::uintmax_t diskSpaceNeeded);
    bool CheckMemory(DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
    bool LoadStrings(std::string language);
    std::wstring GetString(std::wstring sID);

public:
    std::wstring m_saveGameDirectory;
};

