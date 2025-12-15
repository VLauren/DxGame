#include "Game.h"
#include <stdio.h>

void Game::Init()
{
}

void Game::Update(float deltaTime)
{
	for (auto& actor : m_actors) actor->Update(deltaTime);
}
