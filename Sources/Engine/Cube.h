#pragma once

class Cube {
public:
	explicit Cube();
	explicit Cube(DirectX::SimpleMath::Vector3 position);

	std::vector<float> GetVertices() const;
	std::vector<uint32_t> GetIndices() const;

private:
	// Methods
	void PushFace(
		DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 up,
		DirectX::SimpleMath::Vector3 right);

	// Data
	std::vector<float> vertices;
	std::vector<uint32_t> indices;
};
