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

            vec4 distance = curr_light.position - vec4(w_position, 1.0);
            float distanceToLight = length(distance);

            float real_distance = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));
            float att = min(1.0, 1.0f / (curr_light.function.x + real_distance * curr_light.function.y + pow(real_distance, 2) * curr_light.function.z));

            vec4 directionToLight = normalize(curr_light.position - vec4(w_position, 1.0));

            float outer_angle = curr_light.angle;
            float inner_angle = outer_angle - curr_light.penumbra;
            vec3 spot_light_dir = normalize(vec3(-curr_light.direction.x, -curr_light.direction.y, -curr_light.direction.z));
            float x = acos(min(max(dot(vec3(directionToLight), spot_light_dir), -1.0), 1.0));
            float falloff = -2 * pow(((abs(x - inner_angle)) / curr_light.penumbra), 3) + 3 * pow((abs((x - inner_angle)) / curr_light.penumbra), 2);

            float d_dot = max(dot(directionToLight, normalize(vec4(w_normal, 0.0))), 0.0);

            vec3 directionToCamera = -normalize(vec3(camera_pos) - w_position);
            vec3 reflect = reflect(vec3(directionToLight), normalize(w_normal));
            float r_dot = max(dot(reflect, directionToCamera), 0.0);


            if (x <= inner_angle){
                fragColor += curr_light.lightColor * att * diffuse * d_dot;
                fragColor += curr_light.lightColor * att * specular * pow(r_dot, (m_shininess + 0.00001));

            }else if (x > inner_angle && x <= outer_angle){
                fragColor += curr_light.lightColor * att * (1 - falloff) * diffuse * d_dot;
                fragColor += curr_light.lightColor * att * (1 - falloff) * specular * pow(r_dot, (m_shininess + 0.00001));
            }

        }else if (curr_light.type == 2){

            vec4 distance = curr_light.position - vec4(w_position, 1.0);
            float distanceToLight = length(distance);

            float real_distance = sqrt(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2));
            float att = min(1.0, 1.0f / (curr_light.function.x + real_distance * curr_light.function.y + pow(real_distance, 2) * curr_light.function.z));

            vec4 directionToLight = normalize(curr_light.position - vec4(w_position, 1.0));
            float d_dot = max(dot(directionToLight, normalize(vec4(w_normal, 0.0))), 0.0);
            fragColor += curr_light.lightColor * att * diffuse * d_dot;

            vec3 directionToCamera = -normalize(vec3(camera_pos) - w_position);
            vec3 reflect = reflect(vec3(directionToLight), normalize(w_normal));
            float r_dot = max(dot(reflect, directionToCamera), 0.0);
            fragColor += curr_light.lightColor * att * specular * pow(r_dot, (m_shininess + 0.00001));
        }

    }



}
