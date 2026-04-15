struct Particle {
    float4 color;
    float2 position;
    float2 velocity;
};

struct Information {
    float *pDeltaTime;
    Particle *pParticlesIn;
    Particle *pParticlesOut;
};

[vk::push_constant] Information pointers;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeShader(uint3 invocation: SV_DispatchThreadID) {
    uint i = invocation.x;

    Particle updated = pointers.pParticlesIn[i];
    updated.position.x += pointers.pParticlesIn[i].velocity.x * pointers.pDeltaTime[0];
    updated.position.y += pointers.pParticlesIn[i].velocity.y * pointers.pDeltaTime[0];

    if ((updated.position.x <= -1.0) || (updated.position.x >= 1.0)) {
        updated.velocity.x *= -1.0;
    }
    if ((updated.position.y <= -1.0) || (updated.position.y >= 1.0)) {
        updated.velocity.y *= -1.0;
    }

    pointers.pParticlesOut[i] = updated;
}