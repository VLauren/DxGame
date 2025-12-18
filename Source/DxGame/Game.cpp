
#include "imgui.h"
#include "Game.h"
#include "Actors/CubeActor.h"

#include <stdio.h>

std::shared_ptr<CubeActor> testCube, testCube2;

void Game::Init()
{
	auto cubeActor = std::make_shared<CubeActor>(0);
	AddActor(cubeActor);
	auto cubeActor2 = std::make_shared<CubeActor>(0);
	AddActor(cubeActor2);

	testCube = cubeActor;
	testCube2 = cubeActor2;

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

	// testCube->SetPosition({ testCube->GetPosition().x + deltaTime * 0.2f, 0, 0});
	// testCube->SetRotation({ 0, testCube->GetRotation().y + deltaTime * DirectX::XM_PI * 0.05f, 0});

	testCube->SetPosition(pos);
	testCube->SetRotation(rot);
	testCube->SetScale(sca);

}

