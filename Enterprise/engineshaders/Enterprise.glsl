// Enterprise.glsl
// This is the universal header for enterprise shaders.
// Include it in all compiled (non-header) shader files.

// Constants

#define PI 3.141592653589793238462643383279
#define TAU 6.283185307179586476925286766559

// Global Uniform Buffers

layout(std140) uniform EP_PERCAMERA
{
    mat4 ep_matrix_v;
    mat4 ep_matrix_p;
    mat4 ep_matrix_vp;
    vec3 ep_camera_worldpos;
};

layout(std140) uniform EP_PERDRAW
{
    mat4 ep_matrix_m;
    mat4 ep_matrix_mv;
    mat4 ep_matrix_mvp;
};

layout(std140) uniform EP_TIME
{
	vec4 ep_time_real;
	vec4 ep_time_game;
	vec4 ep_time_sinreal;
	vec4 ep_time_singame;
	vec4 ep_time_cosreal;
	vec4 ep_time_cosgame;
};

#ifdef EP_VERTEX
in vec3 ep_position;
#endif
