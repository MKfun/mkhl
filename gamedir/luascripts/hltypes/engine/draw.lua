local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLDraw
local Draw = {}

--- Draws a solid filled rectangle (FillRGBA)
---@param x integer
---@param y integer
---@param width integer
---@param height integer
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
---@param a? integer 0..255 (default 255)
function Draw.box(x, y, width, height, r, g, b, a)
    common.get_api().pfnFillRGBA(
        x, y, width, height,
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b),
        common.clamp_byte(a or 255)
    )
end

--- Draws an alpha-blended rectangle (FillRGBABlend)
---@param x integer
---@param y integer
---@param width integer
---@param height integer
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
---@param a? integer 0..255 (default 255)
function Draw.boxBlend(x, y, width, height, r, g, b, a)
    common.get_api().pfnFillRGBABlend(
        x, y, width, height,
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b),
        common.clamp_byte(a or 255)
    )
end

--- Draws a console font character
---@param x integer
---@param y integer
---@param ch string|integer Character or its ASCII code
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
---@return integer Rendered character width
function Draw.character(x, y, ch, r, g, b)
    local code = type(ch) == "string" and string.byte(ch) or ch
    return common.get_api().pfnDrawCharacter(
        x, y, code,
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b)
    )
end

--- Draws a string with custom RGB color
---@param x integer
---@param y integer
---@param text string
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
---@return integer
function Draw.string(x, y, text, r, g, b)
    return common.get_api().pfnDrawString(
        x, y, tostring(text),
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b)
    )
end

--- Draws a string from right to left (reversed)
---@param x integer
---@param y integer
---@param text string
---@param r integer 0..255
---@param g integer 0..255
---@param b integer 0..255
---@return integer
function Draw.stringReverse(x, y, text, r, g, b)
    return common.get_api().pfnDrawStringReverse(
        x, y, tostring(text),
        common.clamp_byte(r),
        common.clamp_byte(g),
        common.clamp_byte(b)
    )
end

--- Draws a string using console font with current text color
---@param x integer
---@param y integer
---@param text string
---@return integer
function Draw.consoleString(x, y, text)
    return common.get_api().pfnDrawConsoleString(x, y, tostring(text))
end

--- Sets current text color for console string rendering (accepts 0.0..1.0 or 0..255)
---@param r number
---@param g number
---@param b number
function Draw.setTextColor(r, g, b)
    r, g, b = common.normalize_color(r, g, b)
    common.get_api().pfnDrawSetTextColor(r, g, b)
end

--- Retrieves string dimensions in pixels (zero GC allocations)
---@param text string
---@return integer width, integer height
function Draw.getTextSize(text)
    common.buf_int1[0] = 0
    common.buf_int2[0] = 0
    common.get_api().pfnDrawConsoleStringLen(tostring(text), common.buf_int1, common.buf_int2)
    return common.buf_int1[0], common.buf_int2[0]
end

--- Draws a VGUI2 font character
---@param x integer
---@param y integer
---@param ch string|integer
---@param font integer
---@param r? integer If specified, uses DrawCharacterAdd with color
---@param g? integer
---@param b? integer
---@return integer
function Draw.vguiCharacter(x, y, ch, font, r, g, b)
    local code = type(ch) == "string" and string.byte(ch) or ch
    local api = common.get_api()
    if r and g and b then
        return api.pfnVGUI2DrawCharacterAdd(x, y, code, common.clamp_byte(r), common.clamp_byte(g), common.clamp_byte(b), font)
    else
        return api.pfnVGUI2DrawCharacter(x, y, code, font)
    end
end

--- Sets screen color and brightness filter
---@param mode integer
---@param r number
---@param g number
---@param b number
---@param brightness number
function Draw.setFilter(mode, r, g, b, brightness)
    local api = common.get_api()
    r, g, b = common.normalize_color(r, g, b)
    api.pfnSetFilterMode(mode)
    api.pfnSetFilterColor(r, g, b)
    api.pfnSetFilterBrightness(brightness)
end

--- Sets HUD crosshair sprite
---@param hspr integer Sprite handle
---@param rect table|ffi.cdata* Table {left, right, top, bottom} or wrect_t
---@param r integer
---@param g integer
---@param b integer
function Draw.setCrosshair(hspr, rect, r, g, b)
    local rc = common.buf_wrect
    if type(rect) == "table" then
        rc.left = rect.left or rect[1] or 0
        rc.right = rect.right or rect[2] or 0
        rc.top = rect.top or rect[3] or 0
        rc.bottom = rect.bottom or rect[4] or 0
    else
        rc = rect
    end
    common.get_api().pfnSetCrosshair(hspr, rc, common.clamp_byte(r), common.clamp_byte(g), common.clamp_byte(b))
end

--- Retrieves title/text message by name from titles.txt
---@param name string
---@return ffi.cdata*|nil client_textmessage_t*
function Draw.getTextMessage(name)
    local ptr = common.get_api().pfnTextMessageGet(name)
    if ptr == nil or ptr == ffi.NULL then
        return nil
    end
    return ptr
end

return Draw
