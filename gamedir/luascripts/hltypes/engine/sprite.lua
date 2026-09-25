local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLSprite
local Sprite = {}

local function to_wrect_ptr(rect)
    if not rect then
        return ffi.NULL
    end
    if type(rect) == "table" then
        common.buf_wrect.left = rect.left or rect[1] or 0
        common.buf_wrect.right = rect.right or rect[2] or 0
        common.buf_wrect.top = rect.top or rect[3] or 0
        common.buf_wrect.bottom = rect.bottom or rect[4] or 0
        return common.buf_wrect
    end
    return rect
end

--- Loads a sprite from disk path (e.g. "sprites/laserdot.spr")
---@param path string
---@return integer HSPRITE sprite handle
function Sprite.load(path)
    return common.get_api().pfnSPR_Load(tostring(path))
end

--- Loads a map sprite
---@param path string
---@return ffi.cdata* model_s*
function Sprite.loadMap(path)
    return common.get_api().LoadMapSprite(tostring(path))
end

--- Retrieves frame count in sprite
---@param hSprite integer
---@return integer
function Sprite.getFrames(hSprite)
    return common.get_api().pfnSPR_Frames(hSprite)
end

--- Retrieves frame width of sprite
---@param hSprite integer
---@param frame? integer Frame index (default 0)
---@return integer
function Sprite.getWidth(hSprite, frame)
    return common.get_api().pfnSPR_Width(hSprite, frame or 0)
end

--- Retrieves frame height of sprite
---@param hSprite integer
---@param frame? integer Frame index (default 0)
---@return integer
function Sprite.getHeight(hSprite, frame)
    return common.get_api().pfnSPR_Height(hSprite, frame or 0)
end

--- Retrieves frame width and height of sprite
---@param hSprite integer
---@param frame? integer Frame index (default 0)
---@return integer width, integer height
function Sprite.getSize(hSprite, frame)
    local api = common.get_api()
    frame = frame or 0
    return api.pfnSPR_Width(hSprite, frame), api.pfnSPR_Height(hSprite, frame)
end

--- Sets sprite modulation color
---@param hSprite integer
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
function Sprite.setColor(hSprite, r, g, b)
    common.get_api().pfnSPR_Set(
        hSprite,
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b)
    )
end

--- Standard sprite frame rendering
---@param hSprite integer
---@param frame integer Frame index
---@param x integer
---@param y integer
---@param rect? table|ffi.cdata* Optional clipping rect {left, right, top, bottom}
function Sprite.draw(hSprite, frame, x, y, rect)
    Sprite.setColor(hSprite, 255, 255, 255)
    common.get_api().pfnSPR_Draw(frame, x, y, to_wrect_ptr(rect))
end

--- Draws sprite with transparency holes (palette index 255 is transparent)
---@param hSprite integer
---@param frame integer
---@param x integer
---@param y integer
---@param rect? table|ffi.cdata*
function Sprite.drawHoles(hSprite, frame, x, y, rect)
    common.get_api().pfnSPR_DrawHoles(frame, x, y, to_wrect_ptr(rect))
end

--- Draws sprite additively (black background is transparent, colors add up)
---@param hSprite integer
---@param frame integer
---@param x integer
---@param y integer
---@param rect? table|ffi.cdata*
function Sprite.drawAdditive(hSprite, frame, x, y, rect)
    common.get_api().pfnSPR_DrawAdditive(frame, x, y, to_wrect_ptr(rect))
end

--- Generic sprite rendering with explicit blend modes
---@param frame integer
---@param x integer
---@param y integer
---@param rect? table|ffi.cdata*
---@param src integer
---@param dest integer
---@param width integer
---@param height integer
function Sprite.drawGeneric(frame, x, y, rect, src, dest, width, height)
    common.get_api().pfnSPR_DrawGeneric(frame, x, y, to_wrect_ptr(rect), src, dest, width, height)
end

--- Enables scissor clipping rectangle for sprite rendering
---@param x integer
---@param y integer
---@param width integer
---@param height integer
function Sprite.enableScissor(x, y, width, height)
    common.get_api().pfnSPR_EnableScissor(x, y, width, height)
end

--- Disables scissor clipping rectangle
function Sprite.disableScissor()
    common.get_api().pfnSPR_DisableScissor()
end

--- Retrieves pointer to sprite model structure
---@param hSprite integer
---@return ffi.cdata* model_s*
function Sprite.getPointer(hSprite)
    return common.get_api().GetSpritePointer(hSprite)
end

return Sprite
