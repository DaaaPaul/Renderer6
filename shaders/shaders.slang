struct VertexIn {
    [[vk::location(0)]] float4 mPosition;
    [[vk::location(1)]] float4 mColor;
};

struct VertexOut {
    float4 mPosition : SV_Position;
    float4 mColor;
};

struct TransformationMatrices {
    const float4x4 mMODEL;
    const float4x4 mVIEW;
    const float4x4 mPROJECTION;
};

[[vk::binding(0, 0)]]
ConstantBuffer<TransformationMatrices> TRANSFORMATION_MATRICES;

[shader("vertex")]
VertexOut vertexShader(VertexIn vertexData) {
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mMODEL, vertexData.mPosition);
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mVIEW, vertexData.mPosition);
    vertexData.mPosition = mul(TRANSFORMATION_MATRICES.mPROJECTION, vertexData.mPosition);

    return VertexOut(vertexData.mPosition, vertexData.mColor);
}

[shader("fragment")]
float4 fragmentShader(VertexOut interpolatedVertex) {
    return interpolatedVertex.mColor;
}