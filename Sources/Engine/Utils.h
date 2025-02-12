#pragma once

inline DirectX::SimpleMath::Vector4 ToVec4(const DirectX::SimpleMath::Vector3& v) {
	return DirectX::SimpleMath::Vector4(v.x, v.y, v.z, 1.0f);
}

inline DirectX::SimpleMath::Vector4 ToVec4Normal(const DirectX::SimpleMath::Vector3& v) {
	return DirectX::SimpleMath::Vector4(v.x, v.y, v.z, 0.0f);
}

inline float sign(float v) {
	if (v < 0)
		return -1;

	return 1;
}

inline int signInt(int v) {
	if (v < 0)
		return -1;

	return 1;
}

std::vector<std::array<int, 3>> Raycast(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction,
										float maxDistance);
