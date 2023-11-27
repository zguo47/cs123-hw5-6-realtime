#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D samp;

uniform float texelWidth;
uniform float texelHeight;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool filter_or_not;
uniform bool blur_or_not;
uniform bool gray_or_not;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate

    vec2 texelSize = vec2(texelWidth, texelHeight);
    vec4 sum = vec4(0.0);

    if (blur_or_not){
        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                vec2 samplePos = uv + vec2(i, j) * texelSize;
                sum += texture(samp, samplePos);
            }
        }
        fragColor = sum / 25.0;
    }else{
        fragColor = texture(samp, uv);
    }

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if (filter_or_not){
        fragColor = vec4(1.0f - fragColor.x, 1.0f - fragColor.y, 1.0f - fragColor.z, 1);
    }

    if (gray_or_not){
        float color = 0.299 * fragColor.x + 0.587 * fragColor.y + 0.114 * fragColor.z;
        fragColor = vec4(color, color, color, 1);
    }

}
