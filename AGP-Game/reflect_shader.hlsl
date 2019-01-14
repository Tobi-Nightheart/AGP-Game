cbuffer CB0
{
	matrix WVPMatrix; //64 bytes
    matrix WVMatrix; //64bytes
};

TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 texcoord : TEXCOORD;
    float3 normal : NORMAL;
};

VOut ReflectVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	output.position = mul(WVPMatrix, position);

    //position relative to camera
    float3 wvpos = mul(WVMatrix, normal);
    //surface normal relative to camera
    float3 wvnormal = mul(WVMatrix, normal);
    wvnormal = normalize(wvnormal);
    //obtain the reverse eyevector
    float3 eyer = -normalize(wvpos);
    //compute the reflection vector
    output.texcoord = 2.0 * dot(eyer, wvnormal) * wvnormal - eyer;

	return output;
}

float4 ReflectPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
    return cube0.Sample(sampler0, texcoord);
}