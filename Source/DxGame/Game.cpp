#include <stdio.h>
#include "Game.h"
#include "Actors/CubeActor.h"

void Game::Init()
{
	auto cubeActor = std::make_shared<CubeActor>(0);
	AddActor(cubeActor);

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
}

