#pragma once

#include <string>
#include <iostream>
#include <map>

struct GLFWwindow;

class Application
{
public:
	Application(const std::string& title);
	virtual ~Application();
	void Run();

protected:
	virtual void Cleanup();
	virtual bool Initialize();
    virtual bool Load() = 0;
	virtual void Render() = 0;
	virtual void Update() = 0;

	static void HandleResize(GLFWwindow* window, const int32_t width, const int32_t height);
	virtual void OnResize(const int32_t width, const int32_t height);

public:
	[[nodiscard]] GLFWwindow* GetWindow() const;
	[[nodiscard]] int32_t GetWindowWidth() const;
	[[nodiscard]] int32_t GetWindowHeight() const;

protected:
	std::map<std::wstring, std::wstring> m_textResource;

private:
	GLFWwindow* m_window = nullptr;
	int32_t m_width = 0;
	int32_t m_height = 0;
	std::string m_title;

};

