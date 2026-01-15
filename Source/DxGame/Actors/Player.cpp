
#include "Player.h"
#include "../Game.h"
#include "RenderComponents.h"

void Player::Init()
{
	AddComponent(std::make_shared<TextureCubeRenderComponent>(shared_from_this(), m_game->GetScene()));
	Actor::Init();
}

void Player::Update(float deltaTime)
{
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

	Actor::Update(deltaTime);
}
