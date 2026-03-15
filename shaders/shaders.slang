struct VertexIn {
    [[vk::location(0)]] float4 position;
    [[vk::location(1)]] float2 texCoord;
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

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<TransformationMatrices> TRANSFORMATION_MATRICES;
[[vk::binding(0, 1)]]
Sampler2D COMBINED_IMAGE_SAMPLER;

[shader("vertex")]
VertexOut vertexShader(VertexIn vin) {
    VertexOut vout;
    vout.position = mul(TRANSFORMATION_MATRICES.model, vin.position);
    vout.position = mul(TRANSFORMATION_MATRICES.view, vout.position);
    vout.position = mul(TRANSFORMATION_MATRICES.projection, vout.position);
    vout.texCoord = vin.texCoord;

    return vout;
}

[shader("fragment")]
float4 fragmentShader(VertexOut fin) {
    return COMBINED_IMAGE_SAMPLER.Sample(fin.texCoord);
}