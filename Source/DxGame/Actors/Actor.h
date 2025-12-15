#pragma once

#include <vector>
#include <memory>
#include <DirectXMath.h>

class ActorComponent;

class Actor
{
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 rotation = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };

	int m_id;
	std::vector<std::shared_ptr<ActorComponent>> m_components;

public:
	explicit Actor(int id);
	~Actor();

	void Init();

	void Destroy();
	void Update(float deltaTime);

	int GetId() const { return m_id; }

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
};

