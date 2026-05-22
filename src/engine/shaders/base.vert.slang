import layoutAndBindings;
import sharedUtils;
import uniforms;

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    if ((pushConstants.flags & ObjectFlags::PrecomputedMVP) != 0) {
        output.position = mul(pushConstants.model, float4(input.position, 1.0));
    } else {
        output.position = mul(ubo.proj, mul(ubo.view, mul(pushConstants.model, float4(input.position, 1.0))));
    }
    output.tangent = input.tangent;
    output.worldPosition = mul(pushConstants.model, float4(input.position, 1.0)).xyz;
    output.normal = input.normal;
    output.uv = input.uv;
    output.uv2 = input.uv2;
    output.packedColor = input.packedColor;
    return output;
}
