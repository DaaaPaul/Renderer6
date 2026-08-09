struct VSInput {
    float3 position;
    float3 normal;
    float3 tangent;
    float2 uv;
};

struct VSOutput {
    float4 position : SV_Position;
    float3 world_pos;
    float3 normal;
    float3 tangent;
    float2 uv;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4 light_pos[4];
    float4 light_colors[4];
    float4 camera_pos;
};

struct PushConstants {
    float4 base_color_factor;
    float metallic_factor;
    float roughness_factor;
};

static const float DA_PI = 3.14159265359;

[[vk::binding(0, 0)]] ConstantBuffer<UniformBuffer> ubo;
[[vk::binding(1, 0)]] SamplerState sampler;
[[vk::binding(2, 0)]] Texture2D baseColor;
[[vk::binding(3, 0)]] Texture2D metallicRoughness;
[[vk::binding(4, 0)]] Texture2D normal;

[[vk::push_constant]] PushConstants push_constants;

// Normal Distribution Function (D) - GGX/Trowbridge-Reitz Distribution
// Describes the statistical distribution of microfacet orientations
float DistributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness; // Remapping for more perceptual linearity
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float nom = a2; // Numerator: concentration factor
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = DA_PI * denom * denom; // Normalization factor

    return nom / denom; // Normalized distribution
}

// Geometry Function (G) - Smith's method with Schlick-GGX approximation
// Models self-shadowing and masking between microfacets
float GeometrySmith(float NdotV, float NdotL, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Direct lighting remapping

    // Geometry obstruction from view direction (masking)
    float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
    // Geometry obstruction from light direction (shadowing)
    float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

    return ggx1 * ggx2; // Combined masking-shadowing
}

// Fresnel Reflectance (F) - Schlick's approximation
// Models how reflectance changes with viewing angle
float3 FresnelSchlick(float cosTheta, float3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

[[shader("vertex")]]
VSOutput vertex_main(VSInput input) {
    VSOutput output;

    output.position = mul(ubo.projection, mul(ubo.view, mul(ubo.model, float4(input.position, 1.0f))));
    output.world_pos = mul(ubo.model, float4(input.position, 1.0f)).xyz;
    output.normal = mul((float3x3)ubo.model, input.normal);
    output.tangent = mul((float3x3)ubo.model, input.tangent);
    output.uv = input.uv;

	return output;
}

[[shader("fragment")]]
float4 fragment_main(VSOutput input) : SV_Target {
	float4 base_color = baseColor.Sample(sampler, input.uv) * push_constants.base_color_factor;

	float2 metallic_roughness = metallicRoughness.Sample(sampler, input.uv).bg;
	float metallic = metallic_roughness.x * push_constants.metallic_factor;
	float roughness = metallic_roughness.y * push_constants.roughness_factor;

	float3 N = normalize(input.normal);

	/* normals! */
    float3 tangentNormal = normal.Sample(sampler, input.uv).xyz * 2.0 - 1.0;

    // 1. Re-orthogonalize T with respect to N (Gram-Schmidt)
    float3 T = normalize(input.tangent.xyz);
    T = normalize(T - dot(T, N) * N);

    // 2. Calculate B (no need to normalize the cross product if T and N are orthonormal)
    float3 B = cross(N, T);

    // 3. Explicitly transform the tangent normal into world space
    N = normalize(T * tangentNormal.x + B * tangentNormal.y + N * tangentNormal.z);
    /* normals! */

    // === LIGHTING SETUP ===
    // Calculate view direction (fragment to camera)
    float3 V = normalize(ubo.camera_pos.xyz - input.world_pos);

    // Calculate reflection vector for environment mapping
    float3 R = reflect(-V, N);

    // === PBR MATERIAL SETUP ===
    // Calculate F0 (reflectance at normal incidence)
    // Non-metals: low reflectance (~0.04), Metals: colored reflectance from base color
    float3 F0 = float3(0.04, 0.04, 0.04);  // Dielectric default
    F0 = lerp(F0, base_color.rgb, metallic); // Lerp to metallic behavior

    // Initialize outgoing radiance accumulator
    float3 Lo = float3(0.0, 0.0, 0.0);

    // === DIRECT LIGHTING LOOP ===
    // Calculate contribution from each light source
    for (int i = 0; i < 4; i++) {
        float3 lightPos = ubo.light_pos[i].xyz;
        float3 lightColor = ubo.light_colors[i].rgb;

        // Calculate light direction and attenuation
        float3 L = normalize(lightPos - input.world_pos);      // Light direction
        float distance = length(lightPos - input.world_pos);   // Distance for falloff
        float attenuation = 1.0 / (distance * distance);     // Inverse square falloff
        float3 radiance = lightColor * attenuation;           // Attenuated light color

        // Calculate half vector (between view and light directions)
        float3 H = normalize(V + L);

        // === BRDF EVALUATION ===
        // Calculate all necessary dot products for BRDF terms
        float NdotL = max(dot(N, L), 0.0);  // Lambertian falloff
        float NdotV = max(dot(N, V), 0.0);  // View angle
        float NdotH = max(dot(N, H), 0.0);  // Half vector for specular
        float HdotV = max(dot(H, V), 0.0);  // For Fresnel calculation

        // Evaluate Cook-Torrance BRDF components
        float D = DistributionGGX(NdotH, roughness);    // Normal distribution
        float G = GeometrySmith(NdotV, NdotL, roughness); // Geometry function
        float3 F = FresnelSchlick(HdotV, F0);           // Fresnel reflectance

        // Calculate specular BRDF
        float3 numerator = D * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.0001; // Prevent division by zero
        float3 specular = numerator / denominator;

        // === ENERGY CONSERVATION ===
        // Fresnel term represents specular reflection ratio
        float3 kS = F;                          // Specular contribution
        float3 kD = float3(1.0, 1.0, 1.0) - kS; // Diffuse contribution (energy conservation)
        kD *= 1.0 - metallic;                   // Metals have no diffuse reflection

        // === RADIANCE ACCUMULATION ===
        // Combine diffuse (Lambertian) and specular (Cook-Torrance) terms
        // Multiply by incident radiance and cosine foreshortening
        Lo += (kD * base_color.rgb / DA_PI + specular) * radiance * NdotL;
    }

    // Combine all lighting contributions
    float3 color = Lo;

    // === HDR TONE MAPPING AND GAMMA CORRECTION ===
    // Apply Reinhard tone mapping to compress HDR values to [0,1] range
    color = color / (color + float3(1.0, 1.0, 1.0));

    // Output final color with original alpha
    return float4(color, base_color.a);
}