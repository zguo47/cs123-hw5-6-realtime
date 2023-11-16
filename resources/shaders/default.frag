#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 w_position;
in vec3 w_normal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform vec4 ambient;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform vec4 diffuse;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform vec4 specular;
uniform float m_shininess;
uniform vec4 camera_pos;

uniform int num_lights;

struct Light {
    int type;
    vec4 position; // Light position in camera coordinates
    vec4 direction; // Light direction in camera coordinates
    vec4 lightColor;
    vec3 function;
    float penumbra; // Only applicable to spot lights, in RADIANS
    float angle;
};


uniform Light lights[8];

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    fragColor = fragColor + ambient;

    for (int i = 0; i < num_lights; ++i) {
        Light curr_light = lights[i];
        if (curr_light.type == 0){
            vec3 newlightPos = vec3(-curr_light.direction.x, -curr_light.direction.y, -curr_light.direction.z);
            vec3 directionToLight = normalize(newlightPos);
            float d_dot = max(dot(normalize(w_normal), directionToLight), 0.0);
            fragColor += curr_light.lightColor * diffuse * d_dot;

            vec3 directionToCamera = -normalize(vec3(camera_pos) - w_position);
            vec3 reflect = reflect(directionToLight, normalize(w_normal));
            float r_dot = max(dot(reflect, directionToCamera), 0.0);
            fragColor += curr_light.lightColor * specular * pow(r_dot, (m_shininess + 0.00001));
        }else if (curr_light.type == 1){
            continue;
        }else if (curr_light.type == 2){
//            vec4 directionToLight = normalize(curr_light.position - vec4(w_position, 1.0));
//            float d_dot = min(max(dot(directionToLight, normalize(vec4(w_normal, 0.0))), 0.0), 1.0f);
//            fragColor += curr_light.lightColor * diffuse * d_dot;

//            vec4 directionToCamera = -normalize(camera_pos - vec4(w_position, 1.0));
//            vec4 reflect = reflect(directionToLight, normalize(vec4(w_normal, 1.0)));
//            float r_dot = max(dot(reflect, directionToCamera), 0);
//            fragColor += curr_light.lightColor * specular * pow(r_dot, m_shininess);
            continue;
        }

    }



}
