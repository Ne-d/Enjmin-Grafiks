// Buffer 0
cbuffer ModelData: register(b0) {
	float4x4 model;
};

// Buffer 1
cbuffer CameraData: register(b1) {
	float4x4 view;
	float4x4 projection;
}

// Must correspond to the input layout
struct Input {
	float3 pos : POSITION0;
};

// Must correspond to the pixel shader's input
struct Output {
	float4 pos : SV_POSITION;
};

Output main(Input input) {
	Output output;

	output.pos = mul(float4(input.pos, 1), model);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	return output;
}
