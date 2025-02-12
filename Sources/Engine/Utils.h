#pragma once

float sign(float v);

std::vector<std::array<int, 3>> Raycast(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction,
										float maxDistance);
