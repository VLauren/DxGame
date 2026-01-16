#include "CameraActor.h"
#include "../Game.h"
#include "Player.h"
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
	using namespace DirectX;

	if (m_playerRef.expired())
		m_playerRef = m_game->FindActor<Player>();

	DirectX::XMFLOAT3 newPos = m_playerRef.lock()->GetPosition();
	newPos.x += m_offset.x;
	newPos.y += m_offset.y;
	newPos.z = m_offset.z;
	SetPosition(newPos);

	/*
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
	*/

	Actor::Update(deltaTime);
}


