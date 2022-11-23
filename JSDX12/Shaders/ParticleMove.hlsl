//***************************************************************************************
//  ParticleMove.hlsl
//***************************************************************************************


struct Particle
{
    float3 Position;
    float3 Velocity;
    float3 Acceleration;
    float2 size;
};

float TimeStep = 1.0f / 60.0f;
//ConsumeStructuredBuffer<Particle> gInput : register(t0);
//AppendStructuredBuffer<Particle> gOutput : register(u0);


//[numthreads(16, 16, 1)]
//void CS()
//{
// // Consume a data element from the input buffer.
//    Particle p = gInput.Consume();
//    p.Velocity += p.Acceleration * TimeStep;
//    p.Position += p.Velocity * TimeStep;
// // Append normalized vector to output buffer. 
//    gOutput.Append(p);
//}

StructuredBuffer<Particle> gInputA : register(t0);
RWStructuredBuffer<Particle> gOutput : register(u0);

[numthreads(128, 2, 2)]
void CS(int3 dtid : SV_DispatchThreadID)
{
    gOutput[dtid.x].Position = gInputA[dtid.x].Position +gInputA[dtid.x].Velocity;
    //gOutput[dtid.x].Velocity = gInputA[dtid.x].Velocity + gInputA[dtid.x].Acceleration;
    //gOutput[dtid.x].Acceleration = gInputA[dtid.x].Acceleration;
    gOutput[dtid.x].size = gInputA[dtid.x].size;
}
