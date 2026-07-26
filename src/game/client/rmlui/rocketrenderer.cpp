#include "rocketrenderer.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Platform.h>
#include <RmlUi/Core/SystemInterface.h>
#include <algorithm>
#include <string.h>

#if defined _WIN32
#if _MSC_VER >= 1500
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
#elif defined POSIX
#include "glad/glad.h"
#ifdef None
#undef None
#endif
#endif

// --- FBO & Shader Definitions ---
#define BLUR_SIZE 7
#define BLUR_NUM_WEIGHTS ((BLUR_SIZE + 1) / 2)

static const char* shader_frag_passthrough = "#version 120\n"
"uniform sampler2D _tex;\n"
"void main() {\n"
"    gl_FragColor = texture2D(_tex, gl_TexCoord[0].st);\n"
"}\n";

static const char* shader_frag_color_matrix = "#version 120\n"
"uniform sampler2D _tex;\n"
"uniform mat4 _color_matrix;\n"
"void main() {\n"
"    vec4 texColor = texture2D(_tex, gl_TexCoord[0].st);\n"
"    vec3 transformedColor = vec3(_color_matrix * texColor);\n"
"    gl_FragColor = vec4(transformedColor, texColor.a);\n"
"}\n";

static const char* shader_frag_blur = "#version 120\n"
"#define BLUR_SIZE 7\n"
"#define BLUR_NUM_WEIGHTS 4\n"
"uniform sampler2D _tex;\n"
"uniform vec2 _texelOffset;\n"
"uniform float _weights[BLUR_NUM_WEIGHTS];\n"
"uniform vec2 _texCoordMin;\n"
"uniform vec2 _texCoordMax;\n"
"void main() {\n"
"    vec4 color = vec4(0.0);\n"
"    for(int i = 0; i < BLUR_SIZE; i++) {\n"
"        vec2 coord = gl_TexCoord[0].st - float(i - BLUR_NUM_WEIGHTS + 1) * _texelOffset;\n"
"        vec2 in_region = step(_texCoordMin, coord) * step(coord, _texCoordMax);\n"
"        color += texture2D(_tex, coord) * in_region.x * in_region.y * _weights[int(abs(float(i - BLUR_NUM_WEIGHTS + 1)))];\n"
"    }\n"
"    gl_FragColor = color;\n"
"}\n";

static const char* shader_frag_drop_shadow = "#version 120\n"
"uniform sampler2D _tex;\n"
"uniform vec2 _texCoordMin;\n"
"uniform vec2 _texCoordMax;\n"
"uniform vec4 _color;\n"
"void main() {\n"
"    vec2 coord = gl_TexCoord[0].st;\n"
"    vec2 in_region = step(_texCoordMin, coord) * step(coord, _texCoordMax);\n"
"    gl_FragColor = texture2D(_tex, coord).a * in_region.x * in_region.y * _color;\n"
"}\n";

static const char* shader_frag_blend_mask = "#version 120\n"
"uniform sampler2D _tex;\n"
"uniform sampler2D _texMask;\n"
"void main() {\n"
"    vec4 texColor = texture2D(_tex, gl_TexCoord[0].st);\n"
"    float maskAlpha = texture2D(_texMask, gl_TexCoord[0].st).a;\n"
"    gl_FragColor = texColor * maskAlpha;\n"
"}\n";

namespace Gfx {

enum class ProgramId { Passthrough, ColorMatrix, Blur, DropShadow, BlendMask, Count };
enum class UniformId { Tex, ColorMatrix, TexelOffset, TexCoordMin, TexCoordMax, TexMask, Weights, Color, Count };

static const char* const program_uniform_names[(size_t)UniformId::Count] = {
    "_tex", "_color_matrix", "_texelOffset", "_texCoordMin", "_texCoordMax", "_texMask", "_weights[0]", "_color"
};

struct ProgramData {
    GLuint programs[(size_t)ProgramId::Count] = {0};
    GLuint frag_shaders[(size_t)ProgramId::Count] = {0};
    GLint uniforms[(size_t)ProgramId::Count][(size_t)UniformId::Count];
    
    ProgramData() {
        for (int i=0; i<(int)ProgramId::Count; ++i)
            for (int j=0; j<(int)UniformId::Count; ++j)
                uniforms[i][j] = -1;
    }
};

struct FramebufferData {
    int width, height;
    GLuint framebuffer;
    GLuint color_tex_buffer;
    GLuint depth_stencil_buffer;
    bool owns_depth_stencil_buffer;
};

static void CheckGLError(const char* op) {
#ifdef RMLUI_DEBUG
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "OpenGL error during %s. Error code 0x%x.", op, err);
    }
#endif
}

static bool CreateFragmentProgram(ProgramData& data, ProgramId id, const char* code) {
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Shader compile failure");
        glDeleteShader(shader);
        return false;
    }
    
    GLuint prog = glCreateProgram();
    glAttachShader(prog, shader);
    glLinkProgram(prog);
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Program link failure");
        glDeleteProgram(prog);
        glDeleteShader(shader);
        return false;
    }
    
    data.frag_shaders[(size_t)id] = shader;
    data.programs[(size_t)id] = prog;
    
    GLint num_active_uniforms = 0;
    glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &num_active_uniforms);
    char name_buf[64];
    for (int unif = 0; unif < num_active_uniforms; ++unif) {
        GLint array_size = 0;
        GLenum type = 0;
        GLsizei actual_length = 0;
        glGetActiveUniform(prog, unif, 64, &actual_length, &array_size, &type, name_buf);
        GLint location = glGetUniformLocation(prog, name_buf);
        
        for (int i = 0; i < (int)UniformId::Count; i++) {
            if (strcmp(name_buf, program_uniform_names[i]) == 0) {
                data.uniforms[(size_t)id][i] = location;
                break;
            }
        }
    }
    return true;
}

static bool CreateShaders(ProgramData& data) {
    CreateFragmentProgram(data, ProgramId::Passthrough, shader_frag_passthrough);
    CreateFragmentProgram(data, ProgramId::ColorMatrix, shader_frag_color_matrix);
    CreateFragmentProgram(data, ProgramId::Blur, shader_frag_blur);
    CreateFragmentProgram(data, ProgramId::DropShadow, shader_frag_drop_shadow);
    CreateFragmentProgram(data, ProgramId::BlendMask, shader_frag_blend_mask);
    
    glUseProgram(data.programs[(size_t)ProgramId::BlendMask]);
    if(data.uniforms[(size_t)ProgramId::BlendMask][(size_t)UniformId::TexMask] != -1)
        glUniform1i(data.uniforms[(size_t)ProgramId::BlendMask][(size_t)UniformId::TexMask], 1);
    glUseProgram(0);
    return true;
}

static void DestroyShaders(ProgramData& data) {
    for (int i=0; i<(int)ProgramId::Count; i++) {
        if (data.programs[i]) glDeleteProgram(data.programs[i]);
        if (data.frag_shaders[i]) glDeleteShader(data.frag_shaders[i]);
    }
}

static bool CreateFramebuffer(FramebufferData& out_fb, int width, int height, GLuint shared_depth) {
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint color_tex = 0;
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

    GLuint depth_stencil = 0;
    if (shared_depth) {
        depth_stencil = shared_depth;
    } else {
        glGenRenderbuffers(1, &depth_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Rml::Log::Message(Rml::Log::LT_ERROR, "Framebuffer incomplete");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    out_fb.width = width;
    out_fb.height = height;
    out_fb.framebuffer = framebuffer;
    out_fb.color_tex_buffer = color_tex;
    out_fb.depth_stencil_buffer = depth_stencil;
    out_fb.owns_depth_stencil_buffer = !shared_depth;
    return true;
}

static void DestroyFramebuffer(FramebufferData& fb) {
    if (fb.framebuffer) glDeleteFramebuffers(1, &fb.framebuffer);
    if (fb.color_tex_buffer) glDeleteTextures(1, &fb.color_tex_buffer);
    if (fb.owns_depth_stencil_buffer && fb.depth_stencil_buffer) glDeleteRenderbuffers(1, &fb.depth_stencil_buffer);
    fb = {};
}

} // namespace Gfx

// --- RenderLayerStack ---
class RenderLayerStack {
public:
    RenderLayerStack() {}
    ~RenderLayerStack() { DestroyFramebuffers(); }

    Rml::LayerHandle PushLayer() {
        if (layers_size == (int)fb_layers.size()) {
            Gfx::FramebufferData fb = {};
            GLuint shared_depth = fb_layers.empty() ? 0 : fb_layers.front().depth_stencil_buffer;
            Gfx::CreateFramebuffer(fb, width, height, shared_depth);
            fb_layers.push_back(fb);
        }
        layers_size++;
        return GetTopLayerHandle();
    }
    void PopLayer() {
        RMLUI_ASSERT(layers_size > 0);
        layers_size--;
    }
    GLuint base_framebuffer = 0;
    
    const Gfx::FramebufferData& GetLayer(Rml::LayerHandle layer) const {
        if (layer == 0) {
            static Gfx::FramebufferData backbuffer = {};
            backbuffer.framebuffer = base_framebuffer;
            backbuffer.width = width;
            backbuffer.height = height;
            backbuffer.color_tex_buffer = 0;
            return backbuffer;
        }
        return fb_layers[(size_t)layer - 1];
    }
    const Gfx::FramebufferData& GetTopLayer() const {
        return GetLayer(GetTopLayerHandle());
    }
    Rml::LayerHandle GetTopLayerHandle() const {
        return static_cast<Rml::LayerHandle>(layers_size);
    }
    const Gfx::FramebufferData& GetPostprocessPrimary() { return EnsureFramebufferPostprocess(0); }
    const Gfx::FramebufferData& GetPostprocessSecondary() { return EnsureFramebufferPostprocess(1); }
    const Gfx::FramebufferData& GetPostprocessTertiary() { return EnsureFramebufferPostprocess(2); }
    const Gfx::FramebufferData& GetBlendMask() { return EnsureFramebufferPostprocess(3); }

    void SwapPostprocessPrimarySecondary() {
        std::swap(fb_postprocess[0], fb_postprocess[1]);
    }
    void BeginFrame(int new_width, int new_height) {
        RMLUI_ASSERT(layers_size == 0);
        GLint current_fb = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fb);
        base_framebuffer = (GLuint)current_fb;
        
        if (new_width != width || new_height != height) {
            width = new_width;
            height = new_height;
            DestroyFramebuffers();
        }
    }
    void EndFrame() {
        RMLUI_ASSERT(layers_size == 0);
        if (fb_layers.size() > 2) {
            for (size_t i = 2; i < fb_layers.size(); i++)
                Gfx::DestroyFramebuffer(fb_layers[i]);
            fb_layers.resize(2);
        }
    }
private:
    void DestroyFramebuffers() {
        for (auto& fb : fb_layers) Gfx::DestroyFramebuffer(fb);
        for (auto& fb : fb_postprocess) Gfx::DestroyFramebuffer(fb);
        fb_layers.clear();
        fb_postprocess.clear();
        layers_size = 0;
    }
    const Gfx::FramebufferData& EnsureFramebufferPostprocess(int index) {
        while ((int)fb_postprocess.size() <= index) {
            Gfx::FramebufferData fb = {};
            Gfx::CreateFramebuffer(fb, width, height, 0);
            fb_postprocess.push_back(fb);
        }
        return fb_postprocess[index];
    }

    int width = 0, height = 0;
    int layers_size = 0;
    Rml::Vector<Gfx::FramebufferData> fb_layers;
    Rml::Vector<Gfx::FramebufferData> fb_postprocess;
};


// --- Filters Implementation ---
enum class FilterType { Invalid = 0, Passthrough, Blur, DropShadow, ColorMatrix, MaskImage };
struct CompiledFilter {
    FilterType type;
    float blend_factor;
    float sigma;
    Rml::Vector2f offset;
    Rml::ColourbPremultiplied color;
    Rml::Matrix4f color_matrix;
};


// --- RocketRender Base Functions ---
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

RocketRender::RocketRender() {
}
RocketRender::~RocketRender() {
    if (program_data) Gfx::DestroyShaders(*program_data);
}

void RocketRender::PrepareGLState()
{
    if (!program_data) {
        program_data = Rml::MakeUnique<Gfx::ProgramData>();
        Gfx::CreateShaders(*program_data);
    }
    if (!render_layers) {
        render_layers = Rml::MakeUnique<RenderLayerStack>();
    }
    render_layers->BeginFrame(m_width, m_height);

    glDisable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i=0; i<16; i++) glDisableVertexAttribArray(i);

    glDisable(GL_ALPHA_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    glEnable(GL_BLEND);
    glBlendColor(1, 1, 1, 1);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

void RocketRender::EndFrame() {
    if (render_layers) render_layers->EndFrame();
}

struct geometryObjs {
    GLuint vbo;   
    GLuint ibo;   
    int num_indices;
    const GLint* renderable;
};

void RocketRender::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
{
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

	    if (!texture || texture == (Rml::TextureHandle)-2)
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Rml::Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &gobjs->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobjs->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
    gobjs->renderable = indices.data();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return (Rml::CompiledGeometryHandle)gobjs;
}

void RocketRender::EnableScissorRegion(bool enable)
{
    if (enable) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);
}

void RocketRender::SetScissor(Rml::Rectanglei region, bool vertically_flip)
{
    int y = m_height - (region.Position().y + region.Size().y);
    glScissor(region.Position().x, y, region.Size().x, region.Size().y);
}

void RocketRender::SetScissorRegion(Rml::Rectanglei region)
{
    scissor_state = region;
    SetScissor(region, true);
}

#pragma pack(1)
struct TGAHeader {
    char idLength, colourMapType, dataType;
    short int colourMapOrigin, colourMapLength;
    char colourMapDepth;
    short int xOrigin, yOrigin, width, height;
    char bitsPerPixel, imageDescriptor;
};
#pragma pack()

Rml::TextureHandle RocketRender::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
    Rml::FileInterface* file_interface = Rml::GetFileInterface();
    Rml::FileHandle file_handle = file_interface->Open(source);
    if (!file_handle) return false;

    file_interface->Seek(file_handle, 0, SEEK_END);
    size_t buffer_size = file_interface->Tell(file_handle);
    file_interface->Seek(file_handle, 0, SEEK_SET);

    if(buffer_size <= sizeof(TGAHeader)) {
        file_interface->Close(file_handle);
        return false;
    }

    char* buffer = new char[buffer_size];
    file_interface->Read(buffer, buffer_size, file_handle);
    file_interface->Close(file_handle);

    TGAHeader header;
    memcpy(&header, buffer, sizeof(TGAHeader));

    int color_mode = header.bitsPerPixel / 8;
    int image_size = header.width * header.height * 4;

    if (header.dataType != 2 || color_mode < 3) {
        delete[] buffer;
        return false;
    }

    const char* image_src = buffer + sizeof(TGAHeader);
    unsigned char* image_dest = new unsigned char[image_size];

    for (long y = 0; y < header.height; y++) {
        long read_index = y * header.width * color_mode;
        long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
        for (long x = 0; x < header.width; x++) {
            image_dest[write_index] = image_src[read_index+2];
            image_dest[write_index+1] = image_src[read_index+1];
            image_dest[write_index+2] = image_src[read_index];
            if (color_mode == 4) image_dest[write_index+3] = image_src[read_index+3];
            else image_dest[write_index+3] = 255;
            write_index += 4;
            read_index += color_mode;
        }
    }
    texture_dimensions.x = header.width;
    texture_dimensions.y = header.height;
    Rml::TextureHandle result = GenerateTexture(Rml::Span<const Rml::byte>(image_dest, image_size), texture_dimensions);
    delete[] image_dest;
    delete [] buffer;
    return result;
}

Rml::TextureHandle RocketRender::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if (texture_id == 0) return false;

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return (Rml::TextureHandle)texture_id;
}

void RocketRender::ReleaseTexture(Rml::TextureHandle texture) { glDeleteTextures(1, (GLuint*) &texture); }
void RocketRender::ReleaseGeometry(Rml::CompiledGeometryHandle handle) {
    geometryObjs* geometry = (geometryObjs*)handle;
    glDeleteBuffers(1, &geometry->vbo);
    glDeleteBuffers(1, &geometry->ibo);
    delete geometry;
}

void RocketRender::SetTransform(const Rml::Matrix4f *trans)
{
    m_transformEnabled = (bool)trans;
    if (trans) {
        transform = *trans;
        if (std::is_same<Rml::Matrix4f, Rml::ColumnMajorMatrix4f>::value)
            glLoadMatrixf(trans->data());
        else if (std::is_same<Rml::Matrix4f, Rml::RowMajorMatrix4f>::value)
            glLoadMatrixf(trans->Transpose().data());
    } else {
        glLoadIdentity();
    }
}

// --- Layers & Filters Impl ---

Rml::LayerHandle RocketRender::PushLayer() {
    if(!render_layers) return 0;
    Rml::LayerHandle layer = render_layers->PushLayer();
    const Gfx::FramebufferData& fb = render_layers->GetLayer(layer);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.framebuffer);
    
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    
    SetScissor(scissor_state, false); // FBO is not flipped
    return layer;
}

void RocketRender::CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode,
    Rml::Span<const Rml::CompiledFilterHandle> filters)
{
    if(!render_layers) return;
    BlitLayerToPostprocessPrimary(source);
    RenderFilters(filters);
    
    GLuint dest_fb = render_layers->GetLayer(destination).framebuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, dest_fb);
    SetScissor(scissor_state, destination == 0);
    
    glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::Passthrough]);
    glBlendFunc(GL_ONE, blend_mode == Rml::BlendMode::Replace ? GL_ZERO : GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessPrimary().color_tex_buffer);
    DrawFullscreenQuad();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (destination != render_layers->GetTopLayerHandle()) {
        glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetTopLayer().framebuffer);
    }
}

void RocketRender::PopLayer() {
    if(!render_layers) return;
    render_layers->PopLayer();
    GLuint fb = render_layers->GetTopLayer().framebuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    SetScissor(scissor_state, false);
}

Rml::TextureHandle RocketRender::SaveLayerAsTexture() {
    if(!render_layers) return 0;
    const Gfx::FramebufferData& fb = render_layers->GetTopLayer();
    
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fb.width, fb.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.framebuffer);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, fb.width, fb.height);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    
    return (Rml::TextureHandle)tex;
}

Rml::CompiledFilterHandle RocketRender::SaveLayerAsMaskImage() {
    if(!render_layers) return 0;
    Rml::TextureHandle tex = SaveLayerAsTexture();
    if (!tex) return 0;
    
    CompiledFilter filter = {};
    filter.type = FilterType::MaskImage;
    return reinterpret_cast<Rml::CompiledFilterHandle>(new CompiledFilter(std::move(filter)));
}

Rml::CompiledFilterHandle RocketRender::CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) {
    CompiledFilter filter = {};
    if (name == "opacity") {
        filter.type = FilterType::Passthrough;
        filter.blend_factor = Rml::Get(parameters, "value", 1.0f);
    } else if (name == "blur") {
        filter.type = FilterType::Blur;
        filter.sigma = Rml::Get(parameters, "sigma", 1.0f);
    } else if (name == "drop-shadow") {
        filter.type = FilterType::DropShadow;
        filter.sigma = Rml::Get(parameters, "sigma", 0.f);
        filter.color = Rml::Get(parameters, "color", Rml::Colourb()).ToPremultiplied();
        filter.offset = Rml::Get(parameters, "offset", Rml::Vector2f(0.f));
    } else if (name == "brightness") {
        filter.type = FilterType::ColorMatrix;
        float value = Rml::Get(parameters, "value", 1.0f);
        filter.color_matrix = Rml::Matrix4f::Diag(value, value, value, 1.f);
    } else if (name == "contrast") {
        filter.type = FilterType::ColorMatrix;
        const float value = Rml::Get(parameters, "value", 1.0f);
        const float grayness = 0.5f - 0.5f * value;
        filter.color_matrix = Rml::Matrix4f::Diag(value, value, value, 1.f);
        filter.color_matrix.SetColumn(3, Rml::Vector4f(grayness, grayness, grayness, 1.f));
    } else if (name == "invert") {
        filter.type = FilterType::ColorMatrix;
        const float value = Rml::Math::Clamp(Rml::Get(parameters, "value", 1.0f), 0.f, 1.f);
        const float inverted = 1.f - 2.f * value;
        filter.color_matrix = Rml::Matrix4f::Diag(inverted, inverted, inverted, 1.f);
        filter.color_matrix.SetColumn(3, Rml::Vector4f(value, value, value, 1.f));
    } else if (name == "grayscale") {
        filter.type = FilterType::ColorMatrix;
        const float value = Rml::Get(parameters, "value", 1.0f);
        const float rev_value = 1.f - value;
        const Rml::Vector3f gray = value * Rml::Vector3f(0.2126f, 0.7152f, 0.0722f);
        filter.color_matrix = Rml::Matrix4f::FromRows(
            {gray.x + rev_value, gray.y,             gray.z,             0.f},
            {gray.x,             gray.y + rev_value, gray.z,             0.f},
            {gray.x,             gray.y,             gray.z + rev_value, 0.f},
            {0.f,                0.f,                0.f,                1.f}
        );
    }

    if (filter.type != FilterType::Invalid)
        return reinterpret_cast<Rml::CompiledFilterHandle>(new CompiledFilter(std::move(filter)));
    return 0;
}

void RocketRender::ReleaseFilter(Rml::CompiledFilterHandle filter) {
    delete reinterpret_cast<CompiledFilter*>(filter);
}

Rml::CompiledShaderHandle RocketRender::CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) {
    // Custom decorators unsupported natively in this backport yet
    return 0;
}
void RocketRender::RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation, Rml::TextureHandle texture) {
}
void RocketRender::ReleaseShader(Rml::CompiledShaderHandle effect_handle) {
}

void RocketRender::BlitLayerToPostprocessPrimary(Rml::LayerHandle layer_handle) {
    if(!render_layers) return;
    const Gfx::FramebufferData& source = render_layers->GetLayer(layer_handle);
    const Gfx::FramebufferData& destination = render_layers->GetPostprocessPrimary();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source.framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.framebuffer);
    glBlitFramebuffer(0, 0, source.width, source.height, 0, 0, destination.width, destination.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void RocketRender::RenderFilters(Rml::Span<const Rml::CompiledFilterHandle> filter_handles) {
    if(!program_data || !render_layers) return;
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    for (const Rml::CompiledFilterHandle handle : filter_handles) {
        const CompiledFilter& filter = *reinterpret_cast<const CompiledFilter*>(handle);
        
        if (filter.type == FilterType::Passthrough) {
            glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::Passthrough]);
            glBlendFunc(GL_CONSTANT_COLOR, GL_ZERO);
            glBlendColor(filter.blend_factor, filter.blend_factor, filter.blend_factor, filter.blend_factor);
            
            glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessPrimary().color_tex_buffer);
            glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetPostprocessSecondary().framebuffer);
            DrawFullscreenQuad();
            render_layers->SwapPostprocessPrimarySecondary();
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        } else if (filter.type == FilterType::Blur) {
            glDisable(GL_BLEND);
            Rml::Rectanglei win = scissor_state; 
            win.p0.y = m_height - (scissor_state.p0.y + scissor_state.p1.y); 
            RenderBlur(filter.sigma, render_layers->GetPostprocessPrimary(), render_layers->GetPostprocessSecondary(), win);
            glEnable(GL_BLEND);
        } else if (filter.type == FilterType::DropShadow) {
            glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::DropShadow]);
            GLint prog = program_data->programs[(size_t)Gfx::ProgramId::DropShadow];
            glUniform4f(glGetUniformLocation(prog, "_color"), filter.color[0]/255.f, filter.color[1]/255.f, filter.color[2]/255.f, filter.color[3]/255.f);
            glUniform2f(glGetUniformLocation(prog, "_texCoordMin"), scissor_state.p0.x / (float)m_width, (m_height - scissor_state.p1.y) / (float)m_height);
            glUniform2f(glGetUniformLocation(prog, "_texCoordMax"), scissor_state.p1.x / (float)m_width, (m_height - scissor_state.p0.y) / (float)m_height);
            
            glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetPostprocessSecondary().framebuffer);
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessPrimary().color_tex_buffer);
            DrawFullscreenQuad(filter.offset / Rml::Vector2f(m_width, m_height));
            render_layers->SwapPostprocessPrimarySecondary();
            
            glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::Passthrough]);
            glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetPostprocessPrimary().framebuffer);
            glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessSecondary().color_tex_buffer);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            DrawFullscreenQuad();
        } else if (filter.type == FilterType::ColorMatrix) {
            glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::ColorMatrix]);
            glUniformMatrix4fv(glGetUniformLocation(program_data->programs[(size_t)Gfx::ProgramId::ColorMatrix], "_color_matrix"), 1, GL_FALSE, filter.color_matrix.data());
            glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetPostprocessSecondary().framebuffer);
            glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessPrimary().color_tex_buffer);
            DrawFullscreenQuad();
            render_layers->SwapPostprocessPrimarySecondary();
        } else if (filter.type == FilterType::MaskImage) {
            glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::BlendMask]);
            glBindFramebuffer(GL_FRAMEBUFFER, render_layers->GetPostprocessSecondary().framebuffer);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, render_layers->GetPostprocessPrimary().color_tex_buffer);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, render_layers->GetBlendMask().color_tex_buffer);
            DrawFullscreenQuad();
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE0);
            render_layers->SwapPostprocessPrimarySecondary();
        }
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glUseProgram(0);
}

void RocketRender::RenderBlur(float sigma, const Gfx::FramebufferData& source_destination, const Gfx::FramebufferData& temp, Rml::Rectanglei window) {
    if (sigma <= 0.5f) return;
    
    glUseProgram(program_data->programs[(size_t)Gfx::ProgramId::Blur]);
    GLint prog = program_data->programs[(size_t)Gfx::ProgramId::Blur];
    
    float weights[BLUR_NUM_WEIGHTS];
    float sum = 0.0f;
    for (int i = 0; i < BLUR_NUM_WEIGHTS; i++) {
        weights[i] = expf(-float(i * i) / (2.0f * sigma * sigma));
        sum += weights[i] * (i == 0 ? 1.0f : 2.0f);
    }
    for (int i = 0; i < BLUR_NUM_WEIGHTS; i++) weights[i] /= sum;
    
    glUniform1fv(glGetUniformLocation(prog, "_weights[0]"), BLUR_NUM_WEIGHTS, weights);
    glUniform2f(glGetUniformLocation(prog, "_texCoordMin"), window.p0.x / (float)m_width, window.p0.y / (float)m_height);
    glUniform2f(glGetUniformLocation(prog, "_texCoordMax"), window.p1.x / (float)m_width, window.p1.y / (float)m_height);
    
    // Pass 1: horizontal
    glBindFramebuffer(GL_FRAMEBUFFER, temp.framebuffer);
    glBindTexture(GL_TEXTURE_2D, source_destination.color_tex_buffer);
    glUniform2f(glGetUniformLocation(prog, "_texelOffset"), 1.0f / m_width, 0.0f);
    DrawFullscreenQuad();
    
    // Pass 2: vertical
    glBindFramebuffer(GL_FRAMEBUFFER, source_destination.framebuffer);
    glBindTexture(GL_TEXTURE_2D, temp.color_tex_buffer);
    glUniform2f(glGetUniformLocation(prog, "_texelOffset"), 0.0f, 1.0f / m_height);
    DrawFullscreenQuad();
}

void RocketRender::DrawFullscreenQuad() { DrawFullscreenQuad(Rml::Vector2f(0.f), Rml::Vector2f(1.f)); }

void RocketRender::DrawFullscreenQuad(Rml::Vector2f uv_offset, Rml::Vector2f uv_scaling) {
    glEnable(GL_TEXTURE_2D);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glBegin(GL_QUADS);
    glTexCoord2f(uv_offset.x, uv_offset.y); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(uv_offset.x + uv_scaling.x, uv_offset.y); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(uv_offset.x + uv_scaling.x, uv_offset.y + uv_scaling.y); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(uv_offset.x, uv_offset.y + uv_scaling.y); glVertex2f(-1.0f, 1.0f);
    glEnd();
}

#pragma pop_macro("Assert")
