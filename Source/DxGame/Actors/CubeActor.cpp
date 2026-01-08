#include "CubeActor.h"
#include "../Game.h"

CubeActor::CubeActor(int id, Game* game) : Actor(id, game)
{
}

void CubeActor::Init()
{
	auto self = shared_from_this();
	// auto cubeComponent = std::make_shared<CubeRenderComponent>(self, m_game->GetScene());
	auto cubeComponent = std::make_shared<TextureCubeRenderComponent>(self, m_game->GetScene());

	AddComponent(cubeComponent);

	Actor::Init();
}

void CubeActor::Update(float deltaTime)
{
	Actor::Update(deltaTime);
	return;

	auto pos = GetPosition();
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		pos.z += deltaTime * 2;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		pos.x -= deltaTime * 2;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		pos.z -= deltaTime * 2;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		pos.x += deltaTime * 2;
	SetPosition(pos);
}
