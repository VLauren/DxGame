
#include "Player.h"
#include "../Game.h"
#include "RenderComponents.h"
#include "CollisionComponents.h"

void Player::Init()
{
	using namespace DirectX;

	AddComponent(std::make_shared<TextureCubeRenderComponent>(shared_from_this(), m_game->GetScene(), 1, 1, 1));
	AddComponent(std::make_shared<AABBCollisionComponent>(shared_from_this(), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)));
	Actor::Init();
}

void Player::Update(float deltaTime)
{
	using namespace DirectX;

	XMFLOAT2 input{0.0f, 0.0f};

	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		if(axesCount >= 2)
		{
			float leftX = axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			float leftY = axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

			const float dead = 0.15f;
			if (std::abs(leftX) < dead) leftX = 0;
			if (std::abs(leftY) < dead) leftY = 0;

			input.x += leftX;
			input.y -= leftY;
		}
	}

	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		input.y += 1;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		input.x -= 1;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		input.y -= 1;
	if (glfwGetKey(m_game->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		input.x += 1;

	XMStoreFloat2(&input, XMVector2Normalize(XMLoadFloat2(&input)));

	auto pos = GetPosition();
	pos.x += deltaTime * moveSpeed * input.x;
	pos.z += deltaTime * moveSpeed * input.y;
	SetPosition(pos);

	Actor::Update(deltaTime);
}


void Player::OnCollision(Actor* other, const CollisionResult& result)
{
	auto pos = GetPosition();
	pos.x -= result.normal.x * result.penetrationDepth;
	pos.y -= result.normal.y * result.penetrationDepth;
	pos.z -= result.normal.z * result.penetrationDepth;
	SetPosition(pos);
	// TODO non axis aligned normal fix

	// Hack to update renderer world matrix after changing position
	GetComponent<RenderComponent>().lock()->VUpdate(0);
}
