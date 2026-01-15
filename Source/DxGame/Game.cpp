
#include "imgui.h"
#include "Game.h"
#include "Actors/CubeActor.h"
#include "Actors/CameraActor.h"
#include "Actors/Player.h"

#include <stdio.h>

std::shared_ptr<Actor> light;

Game::Game(Scene* scene, GLFWwindow* window) : m_scene(scene), m_window(window) {}

void Game::Init()
{
	// Camera
	// --------
	auto camActor = std::make_shared<CameraActor>(NextId(), this);
	camActor->SetPosition(DirectX::XMFLOAT3(0, 6, -9));
	camActor->SetRotation(DirectX::XMFLOAT3(0.7f, 0, 0));
	AddActor(camActor);

	// light
	// --------
	light = std::make_shared<Actor>(NextId(), this);
	light->SetPosition(DirectX::XMFLOAT3(-2, 2, -2));
	auto lightComponent	 = std::make_shared<LightComponent>(light, GetScene());
	light->AddComponent(lightComponent);
	std::array<float,3> att = { 1.0f, 0.09f, 0.032f };
	lightComponent->VInit(DirectX::XMFLOAT3(1, 0.7f, 0.2f), 1, att );
	AddActor(light);

	// Floor
	// --------
	auto floor = std::make_shared<CubeActor>(NextId(), this);
	floor->SetPosition(DirectX::XMFLOAT3(0, -1.2f, 0));
	floor->SetScale(DirectX::XMFLOAT3(10, 0.2f, 10));
	AddActor(floor);

	// Column
	// --------
	floor = std::make_shared<CubeActor>(NextId(), this);
	floor->SetPosition(DirectX::XMFLOAT3(5, 2, 4));
	floor->SetScale(DirectX::XMFLOAT3(0.5f, 3, 0.5f));
	AddActor(floor);

	// Player
	// --------
	auto player = std::make_shared<Player>(NextId(), this);
	AddActor(player);

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

	/*
	auto pos = testCube->GetPosition();
	auto rot = testCube->GetRotation();
	auto sca = testCube->GetScale();

	ImGui::SetNextWindowSize(ImVec2(800, 125), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
	ImGui::Begin("Cube transform");
	{
		ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&pos), -6, 6);
		ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&rot), -DirectX::XM_PI, DirectX::XM_PI);
		ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&sca), 0, 4);

		if (ImGui::Button("Reset"))
		{
			pos = {0, 0, 0};
			rot = {0, 0, 0};
			sca = {1, 1, 1};
		}
	}
	ImGui::End();

	testCube->SetPosition(pos);
	testCube->SetRotation(rot);
	testCube->SetScale(sca);
	*/

	// ------------

	auto pos = light->GetPosition();
	ImGui::SetNextWindowSize(ImVec2(800, 100), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
	ImGui::Begin("Light");
	{
		ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&pos), -20, 20);
		if (auto lightComponent = light->GetComponent<LightComponent>().lock())
		{
			auto dxCol = lightComponent->GetColour();
			float col[3] = { dxCol.x, dxCol.y, dxCol.z };
			float intensity = lightComponent->GetIntensity();
			ImGui::ColorEdit3("Colour", col);
			ImGui::SliderFloat("Intensity", &intensity, 0, 10);

			lightComponent->SetColour(DirectX::XMFLOAT3(col[0], col[1], col[2]));
			lightComponent->SetIntensity(intensity);
		}
	}
	ImGui::End();

	light->SetPosition(pos);
}

