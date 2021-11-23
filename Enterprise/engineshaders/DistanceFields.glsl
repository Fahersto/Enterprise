// DistanceFields.glsl
// Opacity extraction from signed distance fields
// - opacityFromMSDF(): extract an opacity value from an MSDF texture.
// - opacityFromMSDF_AA(): extract an anti-aliased opacity value from an MSDF texture.

#ifdef EP_FRAGMENT

#define texturePxRange 2.0

float median(float a, float b, float c)
{
    return max(min(a, b), min(max(a, b), c));
}
float screenPxRange(sampler2D tex, vec2 uv)
{
    vec2 unitRange = vec2(texturePxRange)/vec2(textureSize(tex, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(uv);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float opacityFromMSDF(sampler2D tex, vec2 uv, float threshold)
{
    vec3 msd = texture(tex, uv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    return float(sd > threshold);
}

float opacityFromMSDF_AA(sampler2D tex, vec2 uv, float threshold)
{
    vec3 msd = texture(tex, uv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(tex, uv) * (sd - threshold);
    return clamp(screenPxDistance + threshold, 0.0, 1.0);
}

#endif
