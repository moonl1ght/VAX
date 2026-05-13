static const float PI_F = 3.14159265359f;

float3x3 inverse3x3(float3x3 m)
{
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    float invDet = 1.0f / det;
    float3x3 inv;
    inv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invDet;
    inv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
    inv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    inv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
    inv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    inv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invDet;
    inv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invDet;
    inv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invDet;
    inv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invDet;
    return inv;
}