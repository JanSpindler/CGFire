/*#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;


out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    frag_pos = (model_mat * vec4(pos, 1.0)).xyz;
    frag_normal = normal_mat * normal;
    frag_uv = uv;
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0);
}*/

#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in ivec4 boneids;
layout (location = 4) in vec4 boneweights;

const int MAXBONEINFLUENCE = 4;
const int MAXBONES = 100;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;
out vec4 nowpos;
out vec4 prevpos;

uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 finalbones[MAXBONES];
uniform mat4 prevfinalbones[MAXBONES];
uniform mat4 prevPV;
uniform mat4 prevM;

uniform bool use_bone;

void main()
{
    mat4 totalposition = mat4(1.0);
    mat4 prevtotalposition = mat4(1.0f);
    if (use_bone)
    {
        totalposition = mat4(0.0);
        prevtotalposition = mat4(0.0f);
        for (int i=0; i<MAXBONEINFLUENCE; i++){
            if (boneids[i]==-1){
                continue;
            }
            if (boneids[i]>=MAXBONES){
                totalposition = mat4(1.0f);
                prevtotalposition = mat4(1.0f);
                break;
            }
            mat4 localposition = finalbones[boneids[i]]*boneweights[i];
            totalposition += localposition;
            mat4 prevlocalposition = prevfinalbones[boneids[i]]*boneweights[i];
            prevtotalposition += prevlocalposition;
        }
    }
    gl_Position = proj_mat * view_mat * model_mat * totalposition * vec4(pos, 1.0f);
    nowpos = gl_Position;
    prevpos = prevPV * prevM * prevtotalposition * vec4(pos, 1.0f);
    frag_pos = vec3(model_mat * totalposition*vec4(pos, 1.0f));
    frag_normal = normal_mat * vec3(totalposition * vec4(normal, 0));
    frag_uv = uv;
}
