struct Particle {
    float2 position;
    float3 color;
    float2 velocity;
};

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<float> DELTA_TIME;
[[vk::binding(0, 1)]]
StructuredBuffer<Particle> PARTICLES_IN;
[[vk::binding(0, 2)]]
RWStructuredBuffer<Particle> particlesOut;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeShader(uint3 invocation: SV_DispatchThreadID) {
    uint i = invocation.x;

    Particle updated = PARTICLES_IN[i];
    updated.position.x += PARTICLES_IN[i].velocity.x * DELTA_TIME;
    updated.position.y += PARTICLES_IN[i].velocity.y * DELTA_TIME;

    if ((updated.position.x <= -1.0) || (updated.position.x >= 1.0)) {
        updated.velocity.x *= -1.0;
    }
    if ((updated.position.y <= -1.0) || (updated.position.y >= 1.0)) {
        updated.velocity.y *= -1.0;
    }

    particlesOut[i] = updated;
}

struct VertexIn {
    [[vk::location(0)]] float2 position;
    [[vk::location(1)]] float3 color;
};

struct VertexOut {
    float4 position : SV_Position;
    float pointSize : SV_PointSize;
    float3 color : COLOR0;
};

struct FragmentIn {
    float4 position : SV_Position;
    float2 pointCoord : SV_PointCoord;
    float3 color : COLOR0;
};

[shader("vertex")]
VertexOut vertexShader(VertexIn vin) {
    VertexOut vout;
    vout.position = float4(vin.position, 1.0, 1.0);
    vout.pointSize = 14.0;
    vout.color = vin.color;

    return vout;
}

[shader("fragment")]
float4 fragmentShader(FragmentIn in) {
    float2 adjustedCoord = in.pointCoord - float2(0.5, 0.5);
    return float4(in.color, 0.7 - length(adjustedCoord));
}