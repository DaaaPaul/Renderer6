struct VertexIn {
    [[vk::location(0)]] float4 position;
    [[vk::location(1)]] float4 color;
    [[vk::location(2)]] float2 texCoord;
};

struct VertexOut {
    float4 position : SV_Position;
    float4 color;
    float2 texCoord;
};

struct TransformationMatrices {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct DeltaTime {
    float deltaTime;
};

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<TransformationMatrices> TRANSFORMATION_MATRICES;
[[vk::binding(0, 1)]]
ConstantBuffer<DeltaTime> DELTA_TIME;
[[vk::binding(0, 2)]]
Sampler2D combinedImageSampler;

[shader("vertex")]
VertexOut vertexShader(VertexIn vertexData) {
    vertexData.position = mul(TRANSFORMATION_MATRICES.model, vertexData.position);
    vertexData.position = mul(TRANSFORMATION_MATRICES.view, vertexData.position);
    vertexData.position = mul(TRANSFORMATION_MATRICES.projection, vertexData.position);

    return VertexOut(vertexData.position, vertexData.color, vertexData.texCoord);
}

[shader("fragment")]
float4 fragmentShader(VertexOut interpolatedVertex) {
    return combinedImageSampler.Sample(interpolatedVertex.texCoord);
}