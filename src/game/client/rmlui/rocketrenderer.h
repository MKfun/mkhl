    #ifndef KISAKSTRIKE_ROCKETRENDER_H
#define KISAKSTRIKE_ROCKETRENDER_H
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/RenderInterface.h>
#pragma pop_macro("Assert")


class RocketRender : public Rml::RenderInterface
{
private:
    void        *m_glContext;
    int         m_width;
    int         m_height;
    bool        m_transformEnabled;
    /** singleton support **/
public:
    static RocketRender m_Instance;
public:
    RocketRender();

    void PrepareGLState();

    /// Called by RmlUi when it wants to render geometry that it does not wish to optimise.
    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    /// Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
    /// If supported, this should return a handle to an optimised, application-specific version of the data. If
    /// not, do not override the function or return zero; the simpler RenderGeometry() will be called instead.
    virtual 	Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
    /// Called by RmlUi when it wants to enable or disable scissoring to clip content.
    void EnableScissorRegion(bool enable) override;
    /// Called by RmlUi when it wants to change the scissor region.
    void SetScissorRegion(Rml::Rectanglei region) override;
    /// Called by RmlUi when a texture is required by the library.
    virtual Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    /// Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
    /// @param[out] texture_handle The handle to write the texture handle for the generated texture to.
    /// @param[in] source The raw 8-bit texture data. Each pixel is made up of four 8-bit values, indicating red, green, blue and alpha in that order.
    /// @param[in] source_dimensions The dimensions, in pixels, of the source data.
    /// @return True if the texture generation succeeded and the handle is valid, false if not.
    virtual Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    /// Called by RmlUi when a loaded texture is no longer required.
    /// @param texture The texture handle to release.
    virtual void ReleaseTexture(Rml::TextureHandle texture) override;
    virtual void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override {};
    /// Called by RmlUi when it wants to set the current transform matrix to a new matrix.
    virtual void SetTransform(const Rml::Matrix4f *transform) override;

    /** Local Methods **/
    inline void SetScreenSize( int width, int height )
    {
        m_width = width;
        m_height = height;
    }
    inline void SetContext( void *context )
    {
        m_glContext = context;
    }
};



#endif //KISAKSTRIKE_ROCKETRENDER_H
