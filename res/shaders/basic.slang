struct VertexIn {
  float2 position;
}

struct VertexOut {
  float4 position : SV_POSITION;
  float2 uv;
}

ParameterBlock<float2> resolution;

[shader("vertex")]
VertexOut vs_main(VertexIn in) {
  VertexOut out;
  out.position = float4(in.position, 0.0, 1.0);
  out.uv = in.position;

  return out;
}

[shader("fragment")]
float4 fs_main(float2 uv) {
  uv.x *= resolution.x / resolution.y;

  // initialize
  float3 ro = float3(0, 0, -3);
  float3 rd = normalize(float3(uv, 1));
  float3 color = float3(0);

  float t = 0;
  bool hit = false;

  // ray marching
  for (int i = 0; i < 200; i++) {
    float3 pos = ro + rd * t;  // position along the ray
    float d = map(pos);  // current distance to the scene
    t += d;              // march the ray

    if (d < 0.01) {
      hit = true;
      break;
    }
    if (t > 100) break;
  }

  if (hit) {
    color = float3(t * 0.1);
  } else {
    color = float3(0.1, 0.1, 0.2);
  }

  return float4(color, 1);
}

float sdSphere(float3 p, float r) {
  return length(p) - r;
}

float map(float3 pos) {
  float3 spherePos = float3(1, 0, 0);
  float sphere = sdSphere(pos - spherePos, 0.5);

  float ground = pos.y + 1.0;

  return min(sphere, ground);
}
