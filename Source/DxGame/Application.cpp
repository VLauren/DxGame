
#include "Application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_dx11.h"

#include <GLFW/glfw3.h>

#include <chrono>

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
    // ---------

    using clock = std::chrono::steady_clock;
    using seconds = std::chrono::duration<float>;

	clock::time_point last = clock::now();
	while (!glfwWindowShouldClose(m_window))
	{
        clock::time_point now = clock::now();
        float dt = std::chrono::duration_cast<seconds>(now - last).count();
        m_totalTime += dt;

		glfwPollEvents();
		Update(dt);
		Render();

        last = now;
	}

    m_game->Destroy();
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

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

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

     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    return true;
}

void Application::HandleResize(GLFWwindow* window, const int32_t width, const int32_t height)
{
    Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
    application->OnResize(width, height);
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

float Application::GetTotalGameTime() const
{
    return m_totalTime;
}
