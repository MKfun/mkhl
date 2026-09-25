local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLTrace
local Trace = {}

local buf_forward = ffi.new("float[3]")
local buf_right = ffi.new("float[3]")
local buf_up = ffi.new("float[3]")
local buf_angles = ffi.new("float[3]")
local buf_start = ffi.new("float[3]")
local buf_end = ffi.new("float[3]")

--- Calculates directional vectors (forward, right, up) from view angles (pitch, yaw, roll)
---@param pitchOrAngles number|table|number[]
---@param yaw? number
---@param roll? number
---@return {x: number, y: number, z: number} forward, {x: number, y: number, z: number} right, {x: number, y: number, z: number} up
function Trace.angleVectors(pitchOrAngles, yaw, roll)
    if type(pitchOrAngles) == "table" then
        buf_angles[0] = pitchOrAngles[1] or pitchOrAngles.pitch or pitchOrAngles.x or 0
        buf_angles[1] = pitchOrAngles[2] or pitchOrAngles.yaw or pitchOrAngles.y or 0
        buf_angles[2] = pitchOrAngles[3] or pitchOrAngles.roll or pitchOrAngles.z or 0
    else
        buf_angles[0] = pitchOrAngles or 0
        buf_angles[1] = yaw or 0
        buf_angles[2] = roll or 0
    end

    common.get_api().pfnAngleVectors(buf_angles, buf_forward, buf_right, buf_up)

    return { x = buf_forward[0], y = buf_forward[1], z = buf_forward[2] },
           { x = buf_right[0],   y = buf_right[1],   z = buf_right[2]   },
           { x = buf_up[0],      y = buf_up[1],      z = buf_up[2]      }
end

--- Traces a ray in the physical world (PM_TraceLine)
---@param startPos table|number[] {x, y, z}
---@param endPos table|number[] {x, y, z}
---@param flags? integer Trace flags (0 for default, PM_TRACELINE_PHYSENTSONLY, etc.)
---@param hull? integer Player hull (0 - point, 1 - standing, 2 - ducking)
---@param ignoreEnt? integer Entity index to ignore (-1 for none)
---@return { fraction: number, endpos: {x: number, y: number, z: number}, plane: {normal: {x: number, y: number, z: number}, dist: number}, allsolid: boolean, startsolid: boolean, inopen: boolean, inwater: boolean, ent: integer, hitgroup: integer, raw: ffi.cdata* }
function Trace.line(startPos, endPos, flags, hull, ignoreEnt)
    buf_start[0] = startPos[1] or startPos.x or 0
    buf_start[1] = startPos[2] or startPos.y or 0
    buf_start[2] = startPos[3] or startPos.z or 0

    buf_end[0] = endPos[1] or endPos.x or 0
    buf_end[1] = endPos[2] or endPos.y or 0
    buf_end[2] = endPos[3] or endPos.z or 0

    local tr = common.get_api().PM_TraceLine(buf_start, buf_end, flags or 0, hull or 0, ignoreEnt or -1)

    return {
        fraction = tr.fraction,
        endpos = { x = tr.endpos[0], y = tr.endpos[1], z = tr.endpos[2] },
        plane = {
            normal = { x = tr.plane.normal[0], y = tr.plane.normal[1], z = tr.plane.normal[2] },
            dist = tr.plane.dist
        },
        allsolid = tr.allsolid ~= 0,
        startsolid = tr.startsolid ~= 0,
        inopen = tr.inopen ~= 0,
        inwater = tr.inwater ~= 0,
        ent = tr.ent,
        hitgroup = tr.hitgroup,
        raw = tr
    }
end

--- Checks contents of a point in 3D world space (water, solid, empty)
---@param point table|number[] {x, y, z}
---@return integer contents, integer truecontents
function Trace.pointContents(point)
    buf_start[0] = point[1] or point.x or 0
    buf_start[1] = point[2] or point.y or 0
    buf_start[2] = point[3] or point.z or 0

    common.buf_int1[0] = 0
    local contents = common.get_api().PM_PointContents(buf_start, common.buf_int1)
    return contents, common.buf_int1[0]
end

--- Checks if a 3D point is inside a water entity
---@param point table|number[] {x, y, z}
---@return integer
function Trace.waterEntity(point)
    buf_start[0] = point[1] or point.x or 0
    buf_start[1] = point[2] or point.y or 0
    buf_start[2] = point[3] or point.z or 0

    return common.get_api().PM_WaterEntity(buf_start)
end

return Trace
