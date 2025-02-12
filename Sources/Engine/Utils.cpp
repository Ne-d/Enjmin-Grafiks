#include "pch.h"

#include "Utils.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

std::vector<std::array<int, 3>> Raycast(const Vector3 position, const Vector3 direction, const float maxDistance) {
	std::map<float, std::array<int, 3>> cubes;

	if (direction.x != 0) {
		const float deltaYX = direction.y / direction.x;
		const float deltaZX = direction.z / direction.x;
		const float offsetYX = position.y - position.x * deltaYX;
		const float offsetZX = position.z - position.x * deltaZX;

		float cubeX = (direction.x > 0) ? ceil(position.x) : floor(position.x);
		do {
			const Vector3 collision(cubeX, deltaYX * cubeX + offsetYX, deltaZX * cubeX + offsetZX);
			float dist = Vector3::Distance(position, collision);
			if (dist > maxDistance)
				break;

			cubes[dist] = {
				(int)floor(cubeX - ((direction.x < 0) ? 1 : 0)),
				(int)floor(collision.y),
				(int)floor(collision.z)
			};
			cubeX = cubeX + sign(direction.x);
		}
		while (true);
	}
	if (direction.y != 0) {
		const float deltaXY = direction.x / direction.y;
		const float deltaZY = direction.z / direction.y;
		const float offsetXY = position.x - position.y * deltaXY;
		const float offsetZY = position.z - position.y * deltaZY;

		float cubeY = (direction.y > 0) ? ceil(position.y) : floor(position.y);
		do {
			const Vector3 collision(deltaXY * cubeY + offsetXY, cubeY, deltaZY * cubeY + offsetZY);
			float dist = Vector3::Distance(position, collision);
			if (dist > maxDistance)
				break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(cubeY - ((direction.y < 0) ? 1 : 0)),
				(int)floor(collision.z)
			};
			cubeY = cubeY + sign(direction.y);
		}
		while (true);
	}
	if (direction.z != 0) {
		const float deltaXZ = direction.x / direction.z;
		const float deltaYZ = direction.y / direction.z;
		const float offsetXZ = position.x - position.z * deltaXZ;
		const float offsetYZ = position.y - position.z * deltaYZ;

		float cubeZ = (direction.z > 0) ? ceil(position.z) : floor(position.z);
		do {
			const Vector3 collision(deltaXZ * cubeZ + offsetXZ, deltaYZ * cubeZ + offsetYZ, cubeZ);
			float dist = Vector3::Distance(position, collision);
			if (dist > maxDistance)
				break;

			cubes[dist] = {
				(int)floor(collision.x),
				(int)floor(collision.y),
				(int)floor(cubeZ - ((direction.z < 0) ? 1 : 0)),
			};
			cubeZ = cubeZ + sign(direction.z);
		}
		while (true);
	}

	std::vector<std::array<int, 3>> res;
	std::transform(
		cubes.begin(), cubes.end(),
		std::back_inserter(res),
		[](auto& v) { return v.second; });
	return res;
}
