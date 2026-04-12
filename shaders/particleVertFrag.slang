struct VertexIn {
    [vk::location(0)] float4 color;
    [vk::location(1)] float2 position;
};

struct VertexOut {
    float4 position : SV_Position;
    float4 color : COLOR0;
    float pointSize : SV_PointSize;
};

struct FragmentIn {
    float4 position : SV_Position;
    float4 color : COLOR0;
    float2 pointCoord : SV_PointCoord;
};

[shader("vertex")]
VertexOut vertexShader(VertexIn vin) {
    VertexOut vout;
    vout.position = float4(vin.position, 1.0, 1.0);
    vout.color = vin.color;
    vout.pointSize = 14.0;

    return vout;
}

[shader("fragment")]
float4 fragmentShader(FragmentIn in) {
    float2 adjustedCoord = in.pointCoord - float2(0.5, 0.5);
    return float4(in.color.xyz, 0.7 - length(adjustedCoord));
}