cbuffer MatrixData: register(b0) {
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

struct Input {
	float3 pos : POSITION0;
};

struct Output {
	float4 pos : SV_POSITION;
};

Output main(Input input) {
	Output output = (Output)0;

	output.pos = mul(float4(input.pos, 1), model);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	return output;
}