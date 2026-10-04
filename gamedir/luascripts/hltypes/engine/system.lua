local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLSystem
local System = {}

--- Retrieves current client game time (in seconds)
---@return number
function System.getTime()
    return common.get_api().GetClientTime()
end

--- Retrieves high-precision absolute system time (in seconds)
---@return number
function System.getAbsoluteTime()
    return common.get_api().GetAbsoluteTime()
end

--- Retrieves previous client time (old time)
---@return number
function System.getOldTime()
    return common.get_api().hudGetClientOldTime()
end

--- Retrieves server gravity value (sv_gravity)
---@return number
function System.getGravity()
    return common.get_api().hudGetServerGravityValue()
end

--- Retrieves game directory name (e.g. "valve" or "cstrike")
---@return string
function System.getGameDir()
    local ptr = common.get_api().pfnGetGameDirectory()
    return common.c_str(ptr) or ""
end

--- Retrieves current level/map name (e.g. "maps/crossfire.bsp")
---@return string
function System.getLevelName()
    local ptr = common.get_api().pfnGetLevelName()
    return common.c_str(ptr) or ""
end

--- Retrieves Steam AppID of current game
---@return integer
function System.getAppID()
    return common.get_api().pfnGetAppID()
end

--- Generates a pseudo-random floating point number in range [low, high]
---@param low number
---@param high number
---@return number
function System.randomFloat(low, high)
    return common.get_api().pfnRandomFloat(low, high)
end

--- Generates a pseudo-random integer in range [low, high]
---@param low integer
---@param high integer
---@return integer
function System.randomLong(low, high)
    return common.get_api().pfnRandomLong(low, high)
end

--- Retrieves ServerInfo parameter by key
---@param key string
---@return string|nil
function System.getServerInfo(key)
    local ptr = common.get_api().ServerInfo_ValueForKey(tostring(key))
    return common.c_str(ptr)
end

--- Retrieves PhysInfo parameter by key
---@param key string
---@return string|nil
function System.getPhysInfo(key)
    local ptr = common.get_api().PhysInfo_ValueForKey(tostring(key))
    return common.c_str(ptr)
end

--- Loads a 3D model by path
---@param modelName string
---@return ffi.cdata* model_s*, integer index
function System.loadModel(modelName)
    common.buf_int1[0] = 0
    local model_ptr = common.get_api().CL_LoadModel(tostring(modelName), common.buf_int1)
    return model_ptr, common.buf_int1[0]
end

--- Retrieves model pointer by its index
---@param index integer
---@return ffi.cdata* model_s*
function System.getModelByIndex(index)
    return common.get_api().hudGetModelByIndex(index)
end

--- Plays view model weapon animation
---@param anim integer Animation sequence index
---@param body? integer Bodygroup (default 0)
function System.weaponAnim(anim, body)
    common.get_api().pfnWeaponAnim(anim, body or 0)
end

--- Pre-caches a network event
---@param type integer
---@param eventName string
---@return integer Event index
function System.precacheEvent(type, eventName)
    return common.get_api().pfnPrecacheEvent(type, tostring(eventName))
end

return System
