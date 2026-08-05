struct Vertex {
    float2 pos;
    float2 uv;
    float4 color;
}

struct VertexOut {
    float4 pos : SV_Position;
    float4 color;
    float2 uv;
}

struct PushConstantBlock {
    float2 scale;
    float2 translate;
}

[[vk::push_constant]] PushConstantBlock block;
[[vk::binding(0, 0)]] Texture2D texture;
[[vk::binding(1, 0)]] SamplerState sampler;

[[shader("vertex")]]
VertexOut vertex_main(Vertex v) {
    v.pos *= block.scale;
    v.pos += block.translate;

    return VertexOut(float4(v.pos, 1.0, 1.0), v.color, v.uv);
}

[[shader("fragment")]]
float4 fragment_main(VertexOut v_interpolated) : SV_Target {
    return texture.Sample(sampler, v_interpolated.uv) * v_interpolated.color;
}