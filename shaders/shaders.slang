struct VertexIn {
    [[vk::location(0)]] float4 position;
    [[vk::location(1)]] float4 color;
    [[vk::location(2)]] float2 texCoord;
};

struct VertexOut {
    float4 position : SV_Position;
    float pointSize : SV_PointSize;
    float4 color : COLOR0;
    float2 texCoord;
};

struct FragmentIn {
    float4 position : SV_Position;
    float2 pointCoord : SV_PointCoord;
    float4 color : COLOR0;
};

struct TransformationMatrices {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct Particle {
    float4 position;
    float4 color;
    float2 velocity;
};

struct ParticleSSBO {
    Particle particle;
};

struct DeltaTime {
    float deltaTime;
};

// first arguement binding #, second arguement is set #
[[vk::binding(0, 0)]]
ConstantBuffer<TransformationMatrices> TRANSFORMATION_MATRICES;
[[vk::binding(0, 1)]]
Sampler2D COMBINED_IMAGE_SAMPLER;
[[vk::binding(0, 2)]]
ConstantBuffer<DeltaTime> DELTA_TIME;
[[vk::binding(0, 3)]]
StructuredBuffer<ParticleSSBO> PARTICLES_IN;
[[vk::binding(0, 4)]]
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

[shader("vertex")]
VertexOut vertexShader(VertexIn in) {
    VertexOut out;
    out.position = in.position;
    out.pointSize = 14.0;
    out.color = in.color;

    return out;
}

[shader("fragment")]
float4 fragmentShader(FragmentIn in) {
    float2 adjusted = in.pointCoord - 0.5;

    return float4(in.color.xyz, 0.5 - length(adjusted));
}