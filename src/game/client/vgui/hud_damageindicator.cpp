#include "hud_damageindicator.h"
#include "client_vgui.h"
#include "cl_util.h"
#include "triangleapi.h"
using namespace vgui2;
CHudDamageIndicator::CHudDamageIndicator() :
    BaseClass(NULL, "HudDamageIndicator")
{
    SetSize(100, 100); // Silence "parent not sized yet" warning
    SetProportional(true);
    // Panel *pParent = g_pClientMode->GetViewport();
    // SetParent( pParent );

    // SetHiddenBits( HIDEHUD_HEALTH );

    m_WhiteAdditiveMaterial.LoadImage(VGUI2_ROOT_DIR "gfx/hud/ammo_energy.tga");
}

void CHudDamageIndicator::Init( void )
{
    // HOOK_HUD_MESSAGE( CHudDamageIndicator, Damage );
    Reset();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudDamageIndicator::Reset( void )
{
    m_vecDamages.Purge();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudDamageIndicator::VidInit( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudDamageIndicator::OnThink()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CHudDamageIndicator::ShouldDraw( void )
{
    // if ( ShouldDraw() )
    //     return false;

    // Don't draw if we don't have any damage to indicate
    if ( !m_vecDamages.Count() )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Purpose: Convert a damage position in world units to the screen's units
//-----------------------------------------------------------------------------
void CHudDamageIndicator::GetDamagePosition( const Vector &vecDelta, float flRadius, float *xpos, float *ypos, float *flRotation )
{
    // Player Data
    Vector playerPosition, playerAngles;
    // QAngle playerAngles;
    memcpy(playerPosition, gHUD.m_vecOrigin, sizeof(Vector));
    memcpy(playerAngles, gHUD.m_vecAngles, sizeof(Vector));

    Vector forward, right, up(0,0,1);
    // AngleVectors (playerAngles, &forward, NULL, NULL );
    forward.z = 0;
    VectorNormalize(forward);
    CrossProduct( up, forward, right );
    float front = DotProduct(vecDelta, forward);
    float side = DotProduct(vecDelta, right);
    *xpos = flRadius * -side;
    *ypos = flRadius * -front;

    // Get the rotation (yaw)
    *flRotation = atan2(*xpos,*ypos) + M_PI;
    *flRotation *= 180 / M_PI;

    float yawRadians = -(*flRotation) * M_PI / 180.0f;
    float ca = cos( yawRadians );
    float sa = sin( yawRadians );

    // Rotate it around the circle
    *xpos = (int)((ScreenWidth / 2) + (flRadius * sa));
    *ypos = (int)((ScreenHeight / 2) - (flRadius * ca));
}

//-----------------------------------------------------------------------------
// Purpose: Draw a single damage indicator
//-----------------------------------------------------------------------------
void CHudDamageIndicator::DrawDamageIndicator(int x0, int y0, int x1, int y1, float alpha, float flRotation )
{
    // // CMatRenderContextPtr pRenderContext( materials );
    // // IMesh *pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, m_WhiteAdditiveMaterial );

    // Get the corners, since they're being rotated
    int wide = x1 - x0;
    int tall = y1 - y0;
    Vector2D vecCorners[4];
    Vector2D center( x0 + (wide * 0.5f), y0 + (tall * 0.5f) );
    float yawRadians = -flRotation * M_PI / 180.0f;
    Vector2D axis[2];
    axis[0].x = cos(yawRadians);
    axis[0].y = sin(yawRadians);
    axis[1].x = -axis[0].y;
    axis[1].y = axis[0].x;
    Vector2DMA( center, -0.5f * wide, axis[0], vecCorners[0] );
    Vector2DMA( vecCorners[0], -0.5f * tall, axis[1], vecCorners[0] );
    Vector2DMA( vecCorners[0], wide, axis[0], vecCorners[1] );
    Vector2DMA( vecCorners[1], tall, axis[1], vecCorners[2] );
    Vector2DMA( vecCorners[0], tall, axis[1], vecCorners[3] );

    vgui2::VGuiVertex vertices[4];

    int iAlpha = alpha * 255;
    vgui2::surface()->DrawSetColor(255, 255, 255, iAlpha);

    int textureID = vgui2::surface()->CreateNewTextureID();
    vgui2::surface()->DrawSetTextureFile(textureID, "ui/gfx/hud/ammo_energy.tga", true, false);

    vertices[0].x = vecCorners[0].x;
    vertices[0].y = vecCorners[0].y;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = vecCorners[1].x;
    vertices[1].y = vecCorners[1].y;
    vertices[1].u = 0.0f;
    vertices[1].v = 1.0f;

    vertices[2].x = vecCorners[2].x;
    vertices[2].y = vecCorners[2].y;
    vertices[2].u = 1.0f;
    vertices[2].v = 1.0f;

    vertices[3].x = vecCorners[3].x;
    vertices[3].y = vecCorners[3].y;
    vertices[3].u = 1.0f;
    vertices[3].v = 0.0f;

    vgui2::surface()->DrawTexturedPolygon(vertices, 4);
    // pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHudDamageIndicator::Paint()
{
    // Iterate backwards, because we might remove them as we go
    int iSize = m_vecDamages.Count();
    for (int i = iSize-1; i >= 0; i--)
    {
        // Scale size to the damage
        float clampedDamage = clamp( (float) m_vecDamages[i].iScale, 0.f, m_iMaximumDamage );

        int iWidth = RemapVal(clampedDamage, 0, m_iMaximumDamage, m_flMinimumWidth, m_flMaximumWidth) * 0.5;
        int iHeight = RemapVal(clampedDamage, 0, m_iMaximumDamage, m_flMinimumHeight, m_flMaximumHeight) * 0.5;

        // Find the place to draw it
        float xpos, ypos;
        float flRotation;
        float flTimeSinceStart = ( gHUD.m_flTime - m_vecDamages[i].flStartTime );
        float flRadius = RemapVal( min( flTimeSinceStart, m_flTravelTime ), 0, m_flTravelTime, m_flStartRadius, m_flEndRadius );
        GetDamagePosition( m_vecDamages[i].vecDelta, flRadius, &xpos, &ypos, &flRotation );

        // Calculate life left
        float flLifeLeft = ( m_vecDamages[i].flLifeTime - gHUD.m_flTime );
        if ( flLifeLeft > 0 )
        {
            float flPercent = flTimeSinceStart / (m_vecDamages[i].flLifeTime - m_vecDamages[i].flStartTime);
            float alpha;
            if ( flPercent <= m_flFadeOutPercentage )
            {
                alpha = 1.0;
            }
            else
            {
                alpha = 1.0 - RemapVal( flPercent, m_flFadeOutPercentage, 1.0, 0.0, 1.0 );
            }
            DrawDamageIndicator( xpos-iWidth, ypos-iHeight, xpos+iWidth, ypos+iHeight, alpha, flRotation );
        }
        else
        {
            m_vecDamages.Remove(i);
        }
        Msg("Drew!\n");
    }
}
void CHudDamageIndicator::UpdateDamagePanel(int armor, int damageTaken, int bitsDamage, Vector vecFrom)
{
    damage_t damage;
    damage.iScale = damageTaken;
    damage.flStartTime = gHUD.m_flTime;
    damage.flLifeTime = gHUD.m_flTime + RemapVal(damage.iScale, 0, m_iMaximumDamage, m_flMinimumTime, m_flMaximumTime);

    if ( vecFrom == vec3_origin )
    {
        vecFrom = gHUD.m_vecOrigin;
    }

    damage.vecDelta = (vecFrom - gHUD.m_vecOrigin);
    VectorNormalize( damage.vecDelta );

    damage.vecDelta[0] += RandomFloat( -m_flNoise, m_flNoise );
    damage.vecDelta[1] += RandomFloat( -m_flNoise, m_flNoise );
    damage.vecDelta[2] += RandomFloat( -m_flNoise, m_flNoise );
    VectorNormalize( damage.vecDelta );

    m_vecDamages.AddToTail( damage );
}
void CHudDamageIndicator::ApplySchemeSettings(IScheme *pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);
    SetPaintBackgroundEnabled(false);

    // set our size
    int screenWide, screenTall;
    int x, y;
    GetPos(x, y);
    // GetHudSize(screenWide, screenTall);
    SetBounds(0, y, ScreenWidth, ScreenHeight - y);
}

void CHudDamageIndicator::ShowPanel(bool state)
{
    if (state != IsVisible())
    {
        SetVisible(state);
    }
}

vgui2::VPANEL CHudDamageIndicator::GetVPanel()
{
    return BaseClass::GetVPanel();
}

bool CHudDamageIndicator::IsVisible()
{
    return BaseClass::IsVisible();
}

void CHudDamageIndicator::SetParent(vgui2::VPANEL parent)
{
    BaseClass::SetParent(parent);
}
