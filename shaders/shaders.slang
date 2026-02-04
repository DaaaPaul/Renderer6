struct VertexIn {
    [[vk::location(0)]] float4 mColor;
    [[vk::location(1)]] float4 mPosition;
};

struct VertexOut {
    float4 mColor;
    float4 mPosition;
};

[shader("vertex")]
VertexOut vertexShader(VertexIn vertexData) {
    return VertexOut(vertexData.mColor, vertexData.mPosition);
}

[shader("fragment")]
float4 fragmentShader(VertexOut interpolatedVertex) {
    return interpolatedVertex.mColor;
}