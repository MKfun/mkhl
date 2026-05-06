#include "rocketrenderer.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>

#if defined _WIN32
#if _MSC_VER >= 1500 // MSVC++ 9.0 (Visual Studio 2008)
#pragma push_macro("ARRAYSIZE")
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define HSPRITE WINDOWS_HSPRITE
#endif
#pragma comment(lib, "opengl32.lib")
#define GL_GLEXT_PROTOTYPES 1
#include <windows.h>
#include "glad/glad.h"

//#endif
#elif defined POSIX
#include "glad/glad.h"
// #define GL_GLEXT_PROTOTYPES 1
// #include <GL/gl.h>
// #include <GL/glext.h>
// #include <GL/glu.h>
// #include <GLES/gl.h>
// #include <GLES2/gl2.h>
// The None define from X.h conflicts with RmlUi code base,
// use the constant 0L instead where necessary
#ifdef None
#undef None
#endif
#endif
void SaveGLState()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}
void RestoreGLState()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopClientAttrib();

    glPopAttrib();
}
RocketRender RocketRender::m_Instance;

RocketRender::RocketRender() { }

void RocketRender::PrepareGLState()
{
    glDisable(GL_CULL_FACE);

    //make sure to set both of these to zero otherwise mesa will segfault even though it only mentions GL_ARRAY_BUFFER in the docs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // int backup0, backup1, backup2, backup3, backup4, backup5, backup6, backup7, backup8, backup9, backup10,
    //        backup11, backup12, backup13, backup14, backup15;
    // glGetVertexAttribIiv(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup0 );
    // glGetVertexAttribIiv(1, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup1 );
    // glGetVertexAttribIiv(2, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup2 );
    // glGetVertexAttribIiv(3, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup3 );
    // glGetVertexAttribIiv(4, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup4 );
    // glGetVertexAttribiv(5, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup5 );
    // glGetVertexAttribiv(6, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup6 );
    // glGetVertexAttribiv(7, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup7 );
    // glGetVertexAttribiv(8, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup8 );
    // glGetVertexAttribIiv(9, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup9 );
    // glGetVertexAttribIiv(10, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup10 );
    // glGetVertexAttribIiv(11, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup11 );
    // glGetVertexAttribIiv(12, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup12 );
    // glGetVertexAttribIiv(13, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup13 );
    // glGetVertexAttribIiv(14, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup14 );
    // glGetVertexAttribIiv(15, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &backup15 );
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
    glDisableVertexAttribArray(8);
    glDisableVertexAttribArray(9);
    glDisableVertexAttribArray(10);
    glDisableVertexAttribArray(11);
    glDisableVertexAttribArray(12);
    glDisableVertexAttribArray(13);
    glDisableVertexAttribArray(14);
    glDisableVertexAttribArray(15);

    glDisable(GL_ALPHA_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    glEnable(GL_BLEND);
    glBlendColor(1, 1, 1, 1);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    //glStencilFunc( GL_GEQUAL, 253, -1 );
    //glAlphaFunc(GL_GEQUAL, 0);

}
struct geometryObjs {
    GLuint vbo;   // Vertex Buffer Object
    GLuint ibo;   // Index Buffer Object
    int num_indices;
    const GLint* renderable;
};
void RocketRender::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
{
    // RMLUI_UNUSED(num_vertices);
    geometryObjs* geometry = (geometryObjs*)handle;
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_width, m_height, 0, -10000, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glTranslatef(translation.x, translation.y, 0);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ibo);

    glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (const GLvoid*)offsetof(Rml::Vertex, position));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), (const GLvoid*)offsetof(Rml::Vertex, colour));

    if (!texture)
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (const GLvoid*)offsetof(Rml::Vertex, tex_coord));
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_TRIANGLES, geometry->num_indices, GL_UNSIGNED_INT, (const GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glPopMatrix();
}


Rml::CompiledGeometryHandle RocketRender::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
    geometryObjs *gobjs = new geometryObjs();
    gobjs->num_indices = indices.size();
    glGenBuffers(1, &gobjs->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gobjs->vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Rml::Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    // Generate and populate Index Buffer
    glGenBuffers(1, &gobjs->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobjs->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(int),
        indices.data(),
        GL_STATIC_DRAW);
    gobjs->renderable = indices.data();

    // Unbind to be safe
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return (Rml::CompiledGeometryHandle)gobjs;
}

// Called by RmlUi when it wants to enable or disable scissoring to clip content.
void RocketRender::EnableScissorRegion(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

// Called by RmlUi when it wants to change the scissor region.
void RocketRender::SetScissorRegion(Rml::Rectanglei region)
{
    glScissor(
        region.Position().x,
        m_height - (region.Position().y + region.Size().y),
        region.Size().x,
        region.Size().y
        );
}

// Set to byte packing, or the compiler will expand our struct, which means it won't read correctly from file
#pragma pack(1)
struct TGAHeader
{
    char  idLength;
    char  colourMapType;
    char  dataType;
    short int colourMapOrigin;
    short int colourMapLength;
    char  colourMapDepth;
    short int xOrigin;
    short int yOrigin;
    short int width;
    short int height;
    char  bitsPerPixel;
    char  imageDescriptor;
};
// Restore packing
#pragma pack()

Rml::TextureHandle RocketRender::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
    Rml::FileInterface* file_interface = Rml::GetFileInterface();
    Rml::FileHandle file_handle = file_interface->Open(source);
    if (!file_handle)
    {
        return false;
    }

    file_interface->Seek(file_handle, 0, SEEK_END);
    size_t buffer_size = file_interface->Tell(file_handle);
    file_interface->Seek(file_handle, 0, SEEK_SET);

    RMLUI_ASSERTMSG(buffer_size > sizeof(TGAHeader), "Texture file size is smaller than TGAHeader, file must be corrupt or otherwise invalid");
    if(buffer_size <= sizeof(TGAHeader))
    {
        file_interface->Close(file_handle);
        return false;
    }

    char* buffer = new char[buffer_size];
    file_interface->Read(buffer, buffer_size, file_handle);
    file_interface->Close(file_handle);

    TGAHeader header;
    memcpy(&header, buffer, sizeof(TGAHeader));

    int color_mode = header.bitsPerPixel / 8;
    int image_size = header.width * header.height * 4; // We always make 32bit textures

    if (header.dataType != 2)
    {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
        return false;
    }

    // Ensure we have at least 3 colors
    if (color_mode < 3)
    {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Only 24 and 32bit textures are supported");
        return false;
    }

    const char* image_src = buffer + sizeof(TGAHeader);
    unsigned char* image_dest = new unsigned char[image_size];

    // Targa is BGR, swap to RGB and flip Y axis
    for (long y = 0; y < header.height; y++)
    {
        long read_index = y * header.width * color_mode;
        long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
        for (long x = 0; x < header.width; x++)
        {
            image_dest[write_index] = image_src[read_index+2];
            image_dest[write_index+1] = image_src[read_index+1];
            image_dest[write_index+2] = image_src[read_index];
            if (color_mode == 4)
                image_dest[write_index+3] = image_src[read_index+3];
            else
                image_dest[write_index+3] = 255;

            write_index += 4;
            read_index += color_mode;
        }
    }

    texture_dimensions.x = header.width;
    texture_dimensions.y = header.height;
    // bool success = GenerateTexture(Rml::Span<const Rml::byte>(image_dest, image_size), texture_dimensions);

    delete [] image_dest;
    delete [] buffer;

    return GenerateTexture(Rml::Span<const Rml::byte>(image_dest, image_size), texture_dimensions);
}

Rml::TextureHandle RocketRender::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if (texture_id == 0)
    {
        fprintf(stdout,"Failed to generate textures\n");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return (Rml::TextureHandle)texture_id;
    // return true;
}

void RocketRender::ReleaseTexture(Rml::TextureHandle texture)
{
    glDeleteTextures(1, (GLuint*) &texture);
}

void RocketRender::SetTransform(const Rml::Matrix4f *transform)
{
    //TODO: the OpenGL state is not setup right for transforms yet.
    m_transformEnabled = (bool)transform;

    if (transform)
    {
        if (std::is_same<Rml::Matrix4f, Rml::ColumnMajorMatrix4f>::value)
            glLoadMatrixf(transform->data());
        else if (std::is_same<Rml::Matrix4f, Rml::RowMajorMatrix4f>::value)
            glLoadMatrixf(transform->Transpose().data());
    }
    else
        glLoadIdentity();
}
#pragma pop_macro("Assert")
