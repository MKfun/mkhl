#ifndef KISAKSTRIKE_ROCKETRENDER_H
#define KISAKSTRIKE_ROCKETRENDER_H
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/RenderInterface.h>
#pragma pop_macro("Assert")
#include <RmlUi/Core/Types.h>

namespace Gfx {
    struct ProgramData;
    struct FramebufferData;
}
class RenderLayerStack;

class RocketRender : public Rml::RenderInterface
{
private:
    void        *m_glContext;
    int         m_width;
    int         m_height;
    bool        m_transformEnabled;

    // Filters & Layers state
    Rml::UniquePtr<Gfx::ProgramData> program_data;
    Rml::UniquePtr<RenderLayerStack> render_layers;
    Rml::Rectanglei scissor_state;
    Rml::Matrix4f transform;
    Rml::Matrix4f projection;
    
public:
    static RocketRender m_Instance;
public:
    RocketRender();
    virtual ~RocketRender();

    void PrepareGLState();
    
    // Call this at the end of your UI rendering to clean up unused FBOs
    void EndFrame();

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;
    virtual Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    virtual Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    virtual void ReleaseTexture(Rml::TextureHandle texture) override;
    virtual void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override;
    virtual void SetTransform(const Rml::Matrix4f *transform) override;

    // --- Layers & Filters ---
    Rml::LayerHandle PushLayer() override;
    void CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode,
        Rml::Span<const Rml::CompiledFilterHandle> filters) override;
    void PopLayer() override;

    Rml::TextureHandle SaveLayerAsTexture() override;
    Rml::CompiledFilterHandle SaveLayerAsMaskImage() override;

    Rml::CompiledFilterHandle CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void ReleaseFilter(Rml::CompiledFilterHandle filter) override;

    Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void RenderShader(Rml::CompiledShaderHandle shader_handle, Rml::CompiledGeometryHandle geometry_handle, Rml::Vector2f translation,
        Rml::TextureHandle texture) override;
    void ReleaseShader(Rml::CompiledShaderHandle effect_handle) override;

    inline void SetScreenSize( int width, int height )
    {
        m_width = width;
        m_height = height;
    }
    inline void SetContext( void *context )
    {
        m_glContext = context;
    }

private:
    void BlitLayerToPostprocessPrimary(Rml::LayerHandle layer_handle);
    void RenderFilters(Rml::Span<const Rml::CompiledFilterHandle> filter_handles);
    void DrawFullscreenQuad();
    void DrawFullscreenQuad(Rml::Vector2f uv_offset, Rml::Vector2f uv_scaling = Rml::Vector2f(1.f));
    void RenderBlur(float sigma, const Gfx::FramebufferData& source_destination, const Gfx::FramebufferData& temp, Rml::Rectanglei window_flipped);
    void SetScissor(Rml::Rectanglei region, bool vertically_flip = false);
};

#endif //KISAKSTRIKE_ROCKETRENDER_H
