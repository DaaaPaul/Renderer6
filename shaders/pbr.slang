struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 tangent : TANGENT;
};

struct VSOutput {
    float4 position : SV_Position;
    float3 world_pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 tangent : TANGENT;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4 lightPositions[4];
    float4 lightColors[4];
    float4 camPos;
    float exposure;
    float gamma;
    float prefilteredCubeMipLevels;
    float scaleIBLAmbient;
};

struct PushConstants {
    float4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    int baseColorTextureSet;
    int physicalDescriptorTextureSet;
    int normalTextureSet;
    int occlusionTextureSet;
    int emissiveTextureSet;
    float alphaMask;
    float alphaMaskCutoff;
};

static const float DA_PI = 3.14159265359;

[[vk::binding(0, 0)]] ConstantBuffer<UniformBuffer> ubo;
[[vk::binding(1, 0)]] Texture2D baseColorMap;
[[vk::binding(1, 0)]] SamplerState baseColorSampler;
[[vk::binding(2, 0)]] Texture2D metallicRoughnessMap;
[[vk::binding(2, 0)]] SamplerState metallicRoughnessSampler;
[[vk::binding(3, 0)]] Texture2D normalMap;
[[vk::binding(3, 0)]] SamplerState normalSampler;
[[vk::binding(4, 0)]] Texture2D occlusionMap;
[[vk::binding(4, 0)]] SamplerState occlusionSampler;
[[vk::binding(5, 0)]] Texture2D emissiveMap;
[[vk::binding(5, 0)]] SamplerState emissiveSampler;

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
	float4 world_pos = mul(ubo.model, float4(input.position, 1.0f));

    output.position = mul(ubo.projection, mul(ubo.view, world_pos));
    output.world_pos = world_pos.xyz;
	output.normal = mul((float3x3) ubo.model, input.normal);
	output.uv = input.uv;
	output.tangent = input.tangent;

	return output;
}

[[shader("fragment")]]
float4 fragment_main(VSOutput input) : SV_Target {
	float4 base_color = baseColorMap.Sample(baseColorSampler, input.uv) * push_constants.baseColorFactor;

	float2 metallic_roughness = metallicRoughnessMap.Sample(metallicRoughnessSampler, input.uv).bg;
	float metallic = metallic_roughness.x * push_constants.metallicFactor;
	float roughness = metallic_roughness.y * push_constants.roughnessFactor;

	float ambient_occlusion = occlusionMap.Sample(occlusionSampler, input.uv).r;

    float3 emissive = emissiveMap.Sample(emissiveSampler, input.uv).rgb;

	float3 N = normalize(input.normal);

	if(push_constants.normalTextureSet > 0) {
		float3 tangentNormal = normalMap.Sample(normalSampler, input.uv).xyz * 2.0 - 1.0;

        float3 T = normalize(input.tangent.xyz);
        float3 B = normalize(cross(N, T)) * input.tangent.w;
        float3x3 TBN = float3x3(T, B, N);

        // Transform normal from tangent space to world space
        N = normalize(mul(tangentNormal, TBN));
    }

    // === LIGHTING SETUP ===
    // Calculate view direction (fragment to camera)
    float3 V = normalize(ubo.camPos.xyz - input.world_pos);

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
        float3 lightPos = ubo.lightPositions[i].xyz;
        float3 lightColor = ubo.lightColors[i].rgb;

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

    // === AMBIENT AND EMISSIVE ===
    // Add simple ambient lighting (should be replaced with IBL in production)
    float3 ambient = float3(0.03, 0.03, 0.03) * base_color.rgb * ambient_occlusion;

    // Combine all lighting contributions
    float3 color = ambient + Lo + emissive;

    // === HDR TONE MAPPING AND GAMMA CORRECTION ===
    // Apply Reinhard tone mapping to compress HDR values to [0,1] range
    color = color / (color + float3(1.0, 1.0, 1.0));

    // Apply gamma correction for sRGB display (inverse gamma)
    color = pow(color, float3(1.0 / ubo.gamma, 1.0 / ubo.gamma, 1.0 / ubo.gamma));

    // Output final color with original alpha
    return float4(color, base_color.a);
}