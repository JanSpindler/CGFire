#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in ivec4 boneids;
layout (location = 4) in vec4 boneweights;

const int MAXBONEINFLUENCE = 4;
const int MAXBONES = 100;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 finalbones[MAXBONES];
uniform mat4 prevfinalbones[MAXBONES];
uniform mat4 prevPVM;

out vec3 interp_pos;
out vec3 interp_normal;
out vec2 interp_uv;
out vec4 nowpos;
out vec4 prevpos;

void main()
{
    mat4 totalposition = mat4(0.0f);
    mat4 prevtotalposition = mat4(0.0f);
    for (int i=0; i<MAXBONEINFLUENCE; i++){
        if(boneids[i]==-1){
            continue;
        }
        if(boneids[i]>=MAXBONES){
            totalposition = mat4(1.0f);
            break;
        }
        mat4 localposition = finalbones[boneids[i]]*boneweights[i];
        mat4 prevlocalposition = prevfinalbones[boneids[i]]*boneweights[i];
        totalposition += localposition;
        prevtotalposition += prevlocalposition;
    }
    nowpos = proj_mat * view_mat * model_mat * totalposition * vec4(pos, 1.0f);
    gl_Position = nowpos;
    prevpos = prevPVM * prevtotalposition * vec4(pos, 1.0f);
    interp_pos = vec3(model_mat * totalposition*vec4(pos, 1.0f));
    interp_normal = mat3(transpose(inverse(model_mat))) * vec3(totalposition * vec4(normal, 0));
    interp_uv = uv;
}
