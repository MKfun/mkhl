local ffi = require("ffi")
local APIProxy = require("hltypes.APIProxy")

local M = {}

-- Table to anchor C memory and callbacks to prevent garbage collection
M.anchors = {}

--- Anchors an object from garbage collection
---@param obj any
---@return any
function M.anchor(obj)
    table.insert(M.anchors, obj)
    return obj
end

--- Get current pointer to cl_enginefunc_t*
---@return ffi.cdata*
function M.get_api()
    local api = APIProxy.get_engine()
    if not api then
        error("[HL Engine API] Engine is not initialized (GetEngineAPI returned nil). This call is only available inside the game engine.", 2)
    end
    return api
end

-- =========================================================================
-- Static buffers to eliminate GC allocations and pressure in hot render/tick loops
-- =========================================================================
M.buf_int1 = ffi.new("int[1]")
M.buf_int2 = ffi.new("int[1]")
M.buf_float3_1 = ffi.new("float[3]")
M.buf_float3_2 = ffi.new("float[3]")
M.buf_float3_3 = ffi.new("float[3]")

M.buf_screeninfo = ffi.new("SCREENINFO")
M.buf_screeninfo.iSize = ffi.sizeof("SCREENINFO")

M.buf_point = ffi.new("POINT")
M.buf_playerinfo = ffi.new("hud_player_info_t")
M.buf_wrect = ffi.new("wrect_t")
M.buf_char256 = ffi.new("char[256]")
M.buf_char16 = ffi.new("char[16]")

--- Safely converts a C string pointer to a Lua string
---@param c_str ffi.cdata*|nil
---@return string|nil
function M.c_str(c_str)
    if c_str == nil then
        return nil
    end
    return ffi.string(c_str)
end

--- Normalizes color values (converts 0..255 -> 0.0..1.0 if any component > 1)
---@param r number
---@param g number
---@param b number
---@return number, number, number
function M.normalize_color(r, g, b)
    if r > 1 or g > 1 or b > 1 then
        return r / 255, g / 255, b / 255
    end
    return r, g, b
end

--- Clamps color value to byte range 0..255
---@param val number
---@return integer
function M.clamp_byte(val)
    val = math.floor(val or 0)
    if val < 0 then return 0 end
    if val > 255 then return 255 end
    return val
end

return M
