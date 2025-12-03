#include "Application.h"
#include <GLFW/glfw3.h>

Application::Application(const std::string& title)
{
	m_title = title;
}

Application::~Application()
{
	Cleanup();
}

void Application::Run()
{
	if (!Initialize())
		return;

	if (!Load())
		return;

    // Main loop
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		Update();
		Render();
	}
}

void Application::Cleanup()
{
    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool Application::Initialize()
{
    if (!glfwInit())
    {
        printf("GLFW: initialization error\n");
        return false;
    }

    // Basic window for now
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    m_width = static_cast<int32_t>(videoMode->width * 0.7f);
    m_height = static_cast<int32_t>(videoMode->height * 0.7f);

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // GLFW no API to use with DirectX

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    if (m_window == nullptr)
    {
        printf("GLFW: unable to create window\n");
        return false;
    }

    const int32_t wLeft = videoMode->width / 2 - m_width / 2;
    const int32_t wTop = videoMode->height / 2 - m_height / 2;
    glfwSetWindowPos(m_window, wLeft, wTop);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, HandleResize);

    return true;
}

void Application::HandleResize(GLFWwindow* window, const int32_t width, const int32_t height)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
}

void Application::OnResize(const int32_t width, const int32_t height)
{
    m_width = width;
    m_height = height;
}

GLFWwindow* Application::GetWindow() const
{
    return m_window;
}

int32_t Application::GetWindowWidth() const
{
    return m_width;
}

int32_t Application::GetWindowHeight() const
{
    return m_height;
}