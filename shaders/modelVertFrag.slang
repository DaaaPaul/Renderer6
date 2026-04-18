struct VertexIn {
    [vk::location(0)] float4 position;
    [vk::location(1)] float2 texCoord;
};

struct VertexOut {
    float4 position : SV_Position;
    float2 texCoord;
};

struct TransformationMatrices {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

[vk::binding(0, 0)] Texture2D texture;
[vk::binding(1, 0)] SamplerState sampler;
[vk::push_constant] TransformationMatrices *pMatrices;

[shader("vertex")]
VertexOut vertexShader(VertexIn vin) {
    VertexOut vout;
    vout.position = mul(pMatrices->model, vin.position);
    vout.position = mul(pMatrices->view, vout.position);
    vout.position = mul(pMatrices->projection, vout.position);
    vout.texCoord = vin.texCoord;

    return vout;
}

[shader("fragment")]
float4 fragmentShader(VertexOut fin) : SV_Target {
    return texture.Sample(sampler, fin.texCoord);
}