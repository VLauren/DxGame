#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "imgui.h"
#include "Game.h"
#include "Actors/CubeActor.h"
#include "Actors/CameraActor.h"
#include "Actors/Player.h"
#include "Graphics/Graphics.h"

#include <stdio.h>

std::shared_ptr<Actor> light;

Game::Game(Scene* scene, GLFWwindow* window) : m_scene(scene), m_window(window) {}

void Game::Init()
{
    return;
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
	auto floor = std::make_shared<CubeActor>(NextId(), this, std::array<float,3>{10, 0.2f, 7});
	floor->SetPosition(DirectX::XMFLOAT3(0, -1.2f, 2));
	auto col = std::make_shared<AABBCollisionComponent>(floor, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(10, 0.2f, 7));
	floor->AddComponent(col);
	floor->AddComponent(std::make_shared<WireframeCubeRenderComponent>(floor, GetScene()));
	AddActor(floor);

	// Column
	// --------
	floor = std::make_shared<CubeActor>(NextId(), this, std::array<float,3>{1, 2, 1});
	floor->SetPosition(DirectX::XMFLOAT3(5, 1, 2));
	col = std::make_shared<AABBCollisionComponent>(floor, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(1, 2, 1));
	floor->AddComponent(col);
	floor->AddComponent(std::make_shared<WireframeCubeRenderComponent>(floor, GetScene()));
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

	CheckCollisions();

	// ------------

	auto pos = light->GetPosition();
	ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Once);
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

	ImGui::Begin("Colliders");
	{
		ImGui::Checkbox("Show colliders", &Graphics::s_showWireframe);
	}
	ImGui::End();
}

void Game::AddActor(std::shared_ptr<Actor> actor)
{
	m_actors.emplace_back(actor);
}

void Game::RemoveActor(std::shared_ptr<Actor> actor)
{
	std::erase(m_actors, actor);
}

void Game::RegisterCollider(std::weak_ptr<CollisionComponent> c) 
{
	m_colliders.push_back(c);
}

void Game::UnregisterCollider(std::weak_ptr<CollisionComponent> c) 
{
	auto component = c.lock();
	if (component)
	{
		auto pred = [component](const std::weak_ptr<CollisionComponent>& w)
			{
				auto other = w.lock();
				return !other || other == component;
			};

		m_colliders.erase(
			std::remove_if(m_colliders.begin(), m_colliders.end(), pred),
			m_colliders.end());
	}
}

void Game::CheckCollisions() 
{
	for (auto& colA : m_colliders)
	{
		auto colliderA = colA.lock();
		for (auto& colB : m_colliders)
		{
			auto colliderB = colB.lock();

			if (colliderA == colliderB)
				continue;

			CollisionResult result;
			if (colliderA->CheckCollision(*colliderB.get(), &result))
				colliderA->GetOwner().lock()->OnCollision(colliderB->GetOwner().lock().get(), result);
		}
	}
}

