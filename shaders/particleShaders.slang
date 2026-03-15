struct DeltaTime {
    float deltaTime;
};

struct Particle {
    float2 position;
    float3 color;
    float2 velocity;
};

struct ParticleSSBO {
    Particle particle;
};

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<DeltaTime> DELTA_TIME;
[[vk::binding(0, 1)]]
StructuredBuffer<ParticleSSBO> PARTICLES_IN;
[[vk::binding(0, 2)]]
RWStructuredBuffer<ParticleSSBO> particlesOut;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeShader(uint3 invocation: SV_DispatchThreadID) {
    uint i = invocation.x;

    particlesOut[i].particle.position = PARTICLES_IN[i].particle.position;
    particlesOut[i].particle.position.x += PARTICLES_IN[i].particle.velocity.x * DELTA_TIME.deltaTime;
    particlesOut[i].particle.position.y += PARTICLES_IN[i].particle.velocity.y * DELTA_TIME.deltaTime;

    if ((particlesOut[i].particle.position.x <= -1.0) || (particlesOut[i].particle.position.x >= 1.0)) {
        particlesOut[i].particle.velocity.x *= -1.0;
    }
    if ((particlesOut[i].particle.position.y <= -1.0) || (particlesOut[i].particle.position.y >= 1.0)) {
        particlesOut[i].particle.velocity.y *= -1.0;
    }
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
    return float4(in.color, 0.5 - length(adjustedCoord));
}