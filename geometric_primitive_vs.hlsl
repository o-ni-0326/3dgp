#include "geometric_primitive.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD, float4 bone_weights : WEIGHTS, uint4 bone_indices : BONES)
{
	VS_OUT vout;
	vout.position = mul(position, world_view_projection);

	normal.w = 0;
	float4 N = normalize(mul(normal, world));
	float4 L = normalize(-light_direction);

	float4 influence = { 0,0,0,1 };
	for (int i = 0; i < 4; i++) {
		float weight = bone_weights[i];
		if (weight > 0.0f) {
			switch (bone_indices[i])
			{
			case 0: influence.r = weight; break;
			case 1: influence.g = weight; break;
			case 2: influence.b = weight; break;
			}
		}
	}
	vout.color = influence;

	//vout.color = material_color * max(0, dot(L, N));
	vout.color.a = material_color.a;

	vout.texcoord = texcoord;

	return vout;
}