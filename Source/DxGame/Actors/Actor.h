#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <GLFW/glfw3.h>

class ActorComponent;
class Game;

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	explicit Actor(int id, Game* game);
	~Actor();

	virtual void Init();

	void Destroy();
	virtual void Update(float deltaTime);

	int GetId() const { return m_id; }

	void SetPosition(const DirectX::XMFLOAT3& p) { m_position = p; }
	void SetRotation(const DirectX::XMFLOAT3& r) { m_rotation = r; }
	void SetScale	(const DirectX::XMFLOAT3& s) { m_scale = s; }

	DirectX::XMFLOAT3 GetPosition() const { return m_position; }
	DirectX::XMFLOAT3 GetRotation() const { return m_rotation; }
	DirectX::XMFLOAT3 GetScale()	const { return m_scale; }

	template<class T>
	std::weak_ptr<T> GetComponent()
	{
		for (auto& base : m_components)
		{
			std::shared_ptr<T> derived = std::dynamic_pointer_cast<T>(base);
			if (derived)
				return derived; // it returns as a weak pointer
		}

		return std::weak_ptr<T>();
	}

	void AddComponent(std::shared_ptr<ActorComponent> pComponent);

	std::vector<std::shared_ptr<ActorComponent>> GetComponents() { return m_components; };

	virtual void OnCollision(Actor* other) {}

protected:

	int m_id;
	std::vector<std::shared_ptr<ActorComponent>> m_components;

	DirectX::XMFLOAT3 m_position = { 0, 0, 0 };
	DirectX::XMFLOAT3 m_rotation = { 0, 0, 0 };
	DirectX::XMFLOAT3 m_scale = { 1, 1, 1 };

	Game* m_game;
};

