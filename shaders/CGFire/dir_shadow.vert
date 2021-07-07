#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in ivec4 boneids;
layout (location = 4) in vec4 boneweights;

const int MAXBONEINFLUENCE = 4;
const int MAXBONES = 100;

uniform mat4 light_mat;
uniform mat4 model_mat;
uniform mat4 finalbones[MAXBONES];

uniform bool use_bone;

void main()
{
    mat4 totalposition = mat4(1.0);
    if (use_bone)
    {
        totalposition = mat4(0.0);
        for (int i=0; i<MAXBONEINFLUENCE; i++){
            if (boneids[i]==-1){
                continue;
            }
            if (boneids[i]>=MAXBONES){
                totalposition = mat4(1.0f);
                break;
            }
            mat4 localposition = finalbones[boneids[i]]*boneweights[i];
            totalposition += localposition;
        }
    }

    gl_Position = light_mat * model_mat * totalposition * vec4(pos, 1.0);
}
