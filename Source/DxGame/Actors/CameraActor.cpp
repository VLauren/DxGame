#include "CameraActor.h"
#include "../Game.h"
#include "imgui.h"

CameraActor::CameraActor(int id, Game* game) : Actor(id, game)
{
}

void CameraActor::Init()
{
	auto self = shared_from_this();
	auto cameraComponent = std::make_shared<CameraComponent>(self, m_game->GetScene());

	AddComponent(cameraComponent);

	Actor::Init();
}

void CameraActor::Update(float deltaTime)
{

	ImGui::SetNextWindowSize(ImVec2(200, 60), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiCond_Once);
	ImGui::Begin("Camera controls");
	{
		ImGui::ArrowButton("leftButton", ImGuiDir_Left);
		if(ImGui::IsItemActive())
			m_position.x -= deltaTime;

		ImGui::SameLine();
		ImGui::ArrowButton("upButton", ImGuiDir_Up);
		if (ImGui::IsItemActive())
			m_position.y += deltaTime;

		ImGui::SameLine();
		ImGui::ArrowButton("downButton", ImGuiDir_Down);
		if (ImGui::IsItemActive())
			m_position.y -= deltaTime;

		ImGui::SameLine();
		ImGui::ArrowButton("rightButton", ImGuiDir_Right);
		if (ImGui::IsItemActive())
			m_position.x += deltaTime;
	}
	ImGui::End();

	Actor::Update(deltaTime);
}


