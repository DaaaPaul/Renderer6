struct VertexIn {
    [vk::location(0)] float4 position;
    [vk::location(1)] float2 tex_coord;
};

struct VertexOut {
    float4 position : SV_Position;
    float2 tex_coord;
};

struct TransformMatrices {
    float4x4 model_matrix;
    float4x4 view_matrix;
    float4x4 projection_matrix;
};

[vk::binding(0, 0)] Texture2D textures;
[vk::binding(1, 0)] SamplerState samplers;
[vk::push_constant] TransformMatrices* p_transform_matrices;

[shader("vertex")]
VertexOut vertex_shader(VertexIn input) {
    VertexOut output;

    output.position = mul(p_transform_matrices->model_matrix, input.position);
    output.position = mul(p_transform_matrices->view_matrix, output.position);
    output.position = mul(p_transform_matrices->projection_matrix, output.position);
    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
float4 fragment_shader(VertexOut interpolated_output) {
    return textures.Sample(samplers, interpolated_output.tex_coord);
}