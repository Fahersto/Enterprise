// Enterprise.glsl
// This is the universal header for enterprise shaders.
// Include it in all compiled (non-header) shader files.

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
