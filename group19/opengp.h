
/*
 * This file replaces the OpenGP definitions included in common/common.h which
 * also includes declarations (i.e. code). This prevents the inclusion of
 * common.h in multiple compilation units : linker complains about multiple
 * function and constant definitions. This file thus declares (and not defines)
 * the functions and constants from OpenGP used in our project. Their
 * definitions (included from common.h) are solely included in the main
 * compilation unit.
 */

#ifndef __opengp_h__
#define __opengp_h__

// Implemented in external/opengp/include/OpenGP/GL/shader_helpers.h.
namespace opengp {
extern GLuint compile_shaders(const char * vshader,
                       const char* fshader,
                       const char* gshader = NULL,
                       const char* tcshader = NULL,
                       const char* teshader = NULL);
}

/*
 * Implemented in external/opengp/include/OpenGP/GL/glfw_helpers.h.
 * As they are defined there as static, we cannot declare them here as extern.
 * Static means than only the compilation unit where they are defined can
 * reference them. We thus have to redefine them here.
 */
//const bool DONT_NORMALIZE = false;
//const bool DONT_TRANSPOSE = false;
//const int ZERO_STRIDE = 0;
//const void* ZERO_BUFFER_OFFSET = 0;

#endif /* __opengp_h__ */
