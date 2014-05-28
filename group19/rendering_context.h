
#ifndef __rendering_context_h__
#define __rendering_context_h__

#include <string>
#include <map>

/// OpenGL object references.
extern std::map<std::string, unsigned int> framebufferIDs;
extern std::map<std::string, unsigned int> textureIDs;
extern std::map<std::string, unsigned int> viewIDs;

/// Forward declarations (no header includes) (namespace pollution, build time).
class Vertices;

/// @brief OpenGL wrapper class template
class RenderingContext {

protected:

    /// Common methods of all RenderingContext.
    RenderingContext(unsigned int width, unsigned int height);
    void preinit(Vertices* vertices, const char* vshader, const char* fshader, const char* gshader, const char* vertexAttribName1, const char* vertexAttribName2 = NULL);

    void predraw() const;
    void preclean();

    /// Helper function.
    void set_texture(unsigned int textureIndex, int textureID, const char* uniformName, unsigned int target);
    void load_texture(const char * imagepath) const;

    /// Common properties of all RenderingContext.
    const unsigned int _width, _height;
    unsigned int _programID;

    /// Pointer : vertices object can be shared between RenderingContext.
    Vertices* _vertices;

    /// Textures.
    struct texture {
        int ID;
        unsigned int target;
    };
    static const int _nTextures = 20;
    struct texture _textures[_nTextures];

};

#endif /* __rendering_context_h__ */
