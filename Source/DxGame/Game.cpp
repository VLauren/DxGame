#include <stdio.h>
#include "Game.h"
#include "Actors/CubeActor.h"

std::shared_ptr<CubeActor> testCube;

void Game::Init()
{
	auto cubeActor = std::make_shared<CubeActor>(0);
	AddActor(cubeActor);

	testCube = cubeActor;

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

	printf("cube pos x: %f\n", testCube->GetPosition().x);

	testCube->SetPosition({ testCube->GetPosition().x + deltaTime * 0.2f, 0, 0});
	testCube->SetRotation({ 0, testCube->GetRotation().y + deltaTime * DirectX::XM_PI * 0.05f, 0});
}

