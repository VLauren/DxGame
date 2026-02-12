#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

class Skeleton
{
public:
	struct Bone
	{
		std::string name;
		int parentIndex;
		DirectX::XMFLOAT4X4 transform;
	};

private:
	std::vector<Bone> m_bones;
};

