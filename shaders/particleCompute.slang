struct Particle {
    float4 color;
    float2 position;
    float2 velocity;
};

struct PushConstant {
    float *delta;
    Particle *particlesIn;
    Particle *particlesOut;
};

[vk::push_constant] PushConstant pointers;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeShader(uint3 invocation: SV_DispatchThreadID) {
    uint i = invocation.x;

    Particle updated = pointers.particlesIn[i];
    updated.position.x += pointers.particlesIn[i].velocity.x * pointers.delta[0];
    updated.position.y += pointers.particlesIn[i].velocity.y * pointers.delta[0];

    if ((updated.position.x <= -1.0) || (updated.position.x >= 1.0)) {
        updated.velocity.x *= -1.0;
    }
    if ((updated.position.y <= -1.0) || (updated.position.y >= 1.0)) {
        updated.velocity.y *= -1.0;
    }

    pointers.particlesOut[i] = updated;
}