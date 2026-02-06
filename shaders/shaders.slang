struct VertexIn {
    [[vk::location(0)]] float4 mPosition;
    [[vk::location(1)]] float4 mColor;
};

struct VertexOut {
    float4 mPosition : SV_Position;
    float4 mColor;
};

[shader("vertex")]
VertexOut vertexShader(VertexIn vertexData) {
    return VertexOut(vertexData.mPosition, vertexData.mColor);
}

[shader("fragment")]
float4 fragmentShader(VertexOut interpolatedVertex) {
    return interpolatedVertex.mColor;
}