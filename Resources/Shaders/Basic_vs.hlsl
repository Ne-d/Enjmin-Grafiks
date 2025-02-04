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
	float4 pos : POSITION0;
	float2 uv : TEXCOORD0;
};

// Must correspond to the pixel shader's input
struct Output {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Output main(Input input) {
	Output output;

	output.pos = mul(input.pos, model);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.uv = input.uv;

	return output;
}
