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
Sampler2D combinedImageSampler;
[[vk::binding(0, 2)]]
ConstantBuffer<DeltaTime> DELTA_TIME;
[[vk::binding(0, 3)]]
StructuredBuffer<ParticleSSBO> particlesIn;
[[vk::binding(0, 4)]]
RWStructuredBuffer<ParticleSSBO> particlesOut;

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

[shader("compute")]
[numthreads(256, 1, 1)]
void computeShader(uint3 invocation: SV_DispatchThreadID) {
    uint i = invocation.x;

    particlesOut[i].particle.position = particlesIn[i].particle.position;
    particlesOut[i].particle.position.x += particlesIn[i].particle.velocity.x * DELTA_TIME.deltaTime;
    particlesOut[i].particle.position.y += particlesIn[i].particle.velocity.y * DELTA_TIME.deltaTime;

    if ((particlesOut[i].particle.position.x <= -1.0) || (particlesOut[i].particle.position.x >= 1.0)) {
        particlesOut[i].particle.velocity.x *= -1.0;
    }
    if ((particlesOut[i].particle.position.y <= -1.0) || (particlesOut[i].particle.position.y >= 1.0)) {
        particlesOut[i].particle.velocity.y *= -1.0;
    }
}