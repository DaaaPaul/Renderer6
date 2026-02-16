struct VertexIn {
    [[vk::location(0)]] float4 mPosition;
    [[vk::location(1)]] float4 mColor;
    [[vk::location(2)]] float2 mTextureCoordinate;
};

struct VertexOut {
    float4 mPosition : SV_Position;
    float4 mColor;
    float2 mTextureCoordinate;
};

struct TransformationMatrices {
    const float4x4 mMODEL;
    const float4x4 mVIEW;
    const float4x4 mPROJECTION;
};

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<TransformationMatrices> TRANSFORMATION_MATRICES;
[[vk::binding(0, 1)]]
Sampler2D combinedImageSampler;

[shader("vertex")]
VertexOut vertexShader(VertexIn vertexData) {
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mMODEL, vertexData.mPosition);
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mVIEW, vertexData.mPosition);
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mPROJECTION, vertexData.mPosition);

    //const float4x4 CUSTOM = { float4(1, 0, 0, 0), float4(0, -1, 0, 0), float4(0, 0, 0, 1), float4(0, 0, -1, 0) };
    //vertexData.mPosition = mul(CUSTOM, vertexData.mPosition);

    return VertexOut(vertexData.mPosition, vertexData.mColor, vertexData.mTextureCoordinate);
}

[shader("fragment")]
float4 fragmentShader(VertexOut interpolatedVertex) {
    return combinedImageSampler.Sample(interpolatedVertex.mTextureCoordinate);
}