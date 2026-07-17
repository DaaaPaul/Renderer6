struct VSInput {
    float3 pos;
};

struct VSOutput {
    float4 pos : SV_Position;
};

struct PushConstants {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

[[vk::push_constant]] PushConstants push_constants;

[[shader("vertex")]]
VSOutput vertex_main(VSInput vin) {
    VSOutput vout;
    vout.pos = mul(push_constants.projection, mul(push_constants.view, mul(push_constants.model, float4(vin.pos, 1.0f))));

    return vout;
}

[[shader("fragment")]]
float4 fragment_main(VSOutput fin) : SV_Target {
    return float4(1.0, 1.0, 1.0, 1.0);
}