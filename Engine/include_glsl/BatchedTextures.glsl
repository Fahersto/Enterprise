// BatchedTextures.glsl
// Texture sampling support for batched and non-batched renderers
// - BTEX_sampler2D() [global section]
//      Declare a sampler2D (non-batched) or texture index attribute (batched)
// - BTEX_PASS() [vertex shader]
//      Pass a texture ID to the fragment shader (batched)
// - BTEX() [fragment shader]
//      Get named sampler2D (non-batched) or sampler2D associated with index (batched)

#pragma option _ BATCHED_TEXTURES

#ifdef BATCHED_TEXTURES

    #ifdef EP_VERTEX
        // (Awaiting material system)  #define BTEX_sampler2D(name) in int name; flat out int name ## _out;
        #define BTEX_sampler2D(name) in int in_tex; flat out int in_tex_out
        // (Awaiting material system)  #define BTEX_PASS(name) name ## _out = name;
        #define BTEX_PASS(name) in_tex_out = in_tex

    #elif defined(EP_FRAGMENT)
        uniform sampler2D btex_textures[16];
    
        // (Awaiting material system)  #define BTEX_sampler2D(name) flat in int name ## _out
        #define BTEX_sampler2D(name) flat in int in_tex_out
        // (Awaiting material system)  #define BTEX(name) btex_textures[name ## _out]
        #define BTEX(name) btex_textures[in_tex_out]

    #endif

#else // !BATCHED_TEXTURES

    #define BTEX_sampler2D(name) uniform sampler2D name
    #define BTEX_PASS(name) 
    #define BTEX(name) name

#endif
