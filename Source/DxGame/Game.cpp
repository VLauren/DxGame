
#include "imgui.h"
#include "Game.h"
#include "Actors/CubeActor.h"
#include "Actors/CameraActor.h"

#include <stdio.h>

std::shared_ptr<CubeActor> testCube, testCube2;
std::shared_ptr<CameraActor> testCam;
std::shared_ptr<Actor> light;

Game::Game(Scene* scene, GLFWwindow* window) : m_scene(scene), m_window(window) {}

void Game::Init()
{
	auto cubeActor = std::make_shared<CubeActor>(NextId(), this);
	AddActor(cubeActor);
	// auto cubeActor2 = std::make_shared<CubeActor>(NextId(), this);
	// AddActor(cubeActor2);
	auto camActor = std::make_shared<CameraActor>(NextId(), this);
	AddActor(camActor);

	camActor->SetPosition(DirectX::XMFLOAT3(0, 2, -7));
	camActor->SetRotation(DirectX::XMFLOAT3(0.2f, 0, 0));

	testCube = cubeActor;
	// testCube2 = cubeActor2;
	testCam = camActor;

	// light
	// --------
	light = std::make_shared<Actor>(NextId(), this);
	light->SetPosition(DirectX::XMFLOAT3(0, 2, 0));
	auto lightComponent	 = std::make_shared<LightComponent>(light, GetScene());
	light->AddComponent(lightComponent);
	std::array<float,3> att = { 1.0f, 0.09f, 0.032f };
	lightComponent->VInit(DirectX::XMFLOAT3(1, 1, 0), 1, att );

	// Init all actors
	for (auto& actor : m_actors) actor->Init();
}

void Game::Destroy() 
{
	for (auto& actor : m_actors) actor->Destroy();
}

void Game::Update(float deltaTime)
{
	// Update all actors
	for (auto& actor : m_actors) actor->Update(deltaTime);

	auto pos = testCube->GetPosition();
	auto rot = testCube->GetRotation();
	auto sca = testCube->GetScale();

	ImGui::SetNextWindowSize(ImVec2(800, 125), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
	ImGui::Begin("Cube transform");

	ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&pos), -6, 6);
	ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&rot), -DirectX::XM_PI, DirectX::XM_PI);
	ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&sca), 0, 4);

	if (ImGui::Button("Reset"))
	{
		pos = {0, 0, 0};
		rot = {0, 0, 0};
		sca = {1, 1, 1};
	}

	ImGui::End();

	testCube->SetPosition(pos);
	testCube->SetRotation(rot);
	testCube->SetScale(sca);

	// GLFWgamepadstate gp;
	// if (glfwGetGamepadState(GLFW_JOYSTICK_1, &gp))
		// testCam->SetRotation(DirectX::XMFLOAT3(testCam->GetRotation().x + gp.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * deltaTime * 0.3f, 0, 0));

}

