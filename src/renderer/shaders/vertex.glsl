#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 finalBonesMatrices[100];

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;

    // 🔥 Skinning
    vec4 totalPosition = vec4(0.0);

    for(int i = 0; i < 4; i++)
    {
        if(aBoneIDs[i] == -1)
            continue;

        totalPosition +=
            finalBonesMatrices[aBoneIDs[i]] *
            vec4(aPos, 1.0) *
            aWeights[i];
    }

    // fallback if no bones affect vertex
    if(totalPosition == vec4(0.0))
        totalPosition = vec4(aPos, 1.0);

    gl_Position = projection * view * model * totalPosition;
}