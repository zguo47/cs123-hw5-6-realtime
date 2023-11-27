#version 330 core


// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.

layout (location = 0) in vec3 o_position;
layout (location = 1) in vec3 o_normal;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader

out vec3 w_normal;
out vec3 w_position;

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 model_matrix;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat3 normalMatrix;

void main() {

    w_position = vec3(model_matrix * vec4(o_position, 1.0));

    w_normal = normalMatrix * o_normal;

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = projection_matrix * view_matrix * vec4(w_position, 1.0);

}
