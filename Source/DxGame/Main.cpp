
#include <iostream>
#include <GLFW/glfw3.h>

int main()
{
	if (!glfwInit())
	{
		printf("GLFW: initialization error\n");
		return false;
	}

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	int width = static_cast<int32_t>(videoMode->width * 0.7f);
	int height = static_cast<int32_t>(videoMode->height * 0.7f);

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(width, height, "Test window", nullptr, nullptr);

	if (window == nullptr)
	{
		printf("GLFW: unable to create window\n");
		return false;
	}

	const int32_t wLeft = videoMode->width / 2 - width / 2;
	const int32_t wTop = videoMode->height / 2 - height / 2;
	glfwSetWindowPos(window, wLeft, wTop);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	return 0;
}

