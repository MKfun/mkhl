local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLScreen
local Screen = {}

--- Retrieves detailed screen metrics and flags
---@return { width: integer, height: integer, charHeight: integer, flags: integer }
function Screen.getInfo()
    local api = common.get_api()
    common.buf_screeninfo.iSize = ffi.sizeof("SCREENINFO")
    api.pfnGetScreenInfo(common.buf_screeninfo)
    return {
        width = common.buf_screeninfo.iWidth,
        height = common.buf_screeninfo.iHeight,
        charHeight = common.buf_screeninfo.iCharHeight,
        flags = common.buf_screeninfo.iFlags
    }
end

--- Retrieves screen width and height in pixels
---@return integer width, integer height
function Screen.getSize()
    local info = Screen.getInfo()
    return info.width, info.height
end

--- Retrieves screen width in pixels
---@return integer
function Screen.getWidth()
    return Screen.getInfo().width
end

--- Retrieves screen height in pixels
---@return integer
function Screen.getHeight()
    return Screen.getInfo().height
end

--- Retrieves center coordinates of game window
---@return integer centerX, integer centerY
function Screen.getCenter()
    local api = common.get_api()
    return api.GetWindowCenterX(), api.GetWindowCenterY()
end

--- Retrieves camera view angles (pitch, yaw, roll)
---@return number pitch, number yaw, number roll
function Screen.getViewAngles()
    local api = common.get_api()
    api.GetViewAngles(common.buf_float3_1)
    return common.buf_float3_1[0], common.buf_float3_1[1], common.buf_float3_1[2]
end

--- Sets camera view angles
--- Accepts 3 numbers (pitch, yaw, roll) or table/array {pitch, yaw, roll}
---@param pitchOrAngles number|table|number[]
---@param yaw? number
---@param roll? number
function Screen.setViewAngles(pitchOrAngles, yaw, roll)
    if type(pitchOrAngles) == "table" then
        common.buf_float3_1[0] = pitchOrAngles[1] or pitchOrAngles.pitch or pitchOrAngles.x or 0
        common.buf_float3_1[1] = pitchOrAngles[2] or pitchOrAngles.yaw or pitchOrAngles.y or 0
        common.buf_float3_1[2] = pitchOrAngles[3] or pitchOrAngles.roll or pitchOrAngles.z or 0
    else
        common.buf_float3_1[0] = pitchOrAngles or 0
        common.buf_float3_1[1] = yaw or 0
        common.buf_float3_1[2] = roll or 0
    end
    common.get_api().SetViewAngles(common.buf_float3_1)
end

--- Calculates view screen shake
function Screen.calcShake()
    common.get_api().V_CalcShake()
end

--- Applies screen shake to position and view angles
---@param origin table|number[] {x, y, z}
---@param angles table|number[] {pitch, yaw, roll}
---@param factor number
---@return table newOrigin, table newAngles
function Screen.applyShake(origin, angles, factor)
    common.buf_float3_1[0] = origin[1] or origin.x or 0
    common.buf_float3_1[1] = origin[2] or origin.y or 0
    common.buf_float3_1[2] = origin[3] or origin.z or 0

    common.buf_float3_2[0] = angles[1] or angles.x or 0
    common.buf_float3_2[1] = angles[2] or angles.y or 0
    common.buf_float3_2[2] = angles[3] or angles.z or 0

    common.get_api().V_ApplyShake(common.buf_float3_1, common.buf_float3_2, factor or 1.0)

    return {
        x = common.buf_float3_1[0],
        y = common.buf_float3_1[1],
        z = common.buf_float3_1[2]
    }, {
        pitch = common.buf_float3_2[0],
        yaw = common.buf_float3_2[1],
        roll = common.buf_float3_2[2]
    }
end

return Screen
