local ffi = require("ffi")
local common = require("hltypes.engine.common")
local cvardef = require("hltypes.cvardef")

---@class HLCVar
local CVar = {
    flags = cvardef.flags
}

--- Get floating-point value of a console variable
---@param name string
---@return number
function CVar.getFloat(name)
    return common.get_api().pfnGetCvarFloat(name)
end

--- Get string value of a console variable
---@param name string
---@return string
function CVar.getString(name)
    local ptr = common.get_api().pfnGetCvarString(name)
    return common.c_str(ptr) or ""
end

--- Get integer value of a console variable
---@param name string
---@return integer
function CVar.getInt(name)
    return math.floor(CVar.getFloat(name))
end

--- Get boolean value of a console variable
---@param name string
---@return boolean
function CVar.getBool(name)
    return CVar.getFloat(name) ~= 0
end

--- Set cvar value (accepts string, number, or boolean)
---@param name string
---@param value string|number|boolean
function CVar.set(name, value)
    local api = common.get_api()
    if type(value) == "number" then
        api.Cvar_SetValue(name, value)
    elseif type(value) == "boolean" then
        api.Cvar_SetValue(name, value and 1.0 or 0.0)
    else
        api.Cvar_Set(name, tostring(value))
    end
end

--- Set float value directly
---@param name string
---@param value number
function CVar.setValue(name, value)
    common.get_api().Cvar_SetValue(name, tonumber(value) or 0)
end

--- Finds pointer to cvar_t structure
---@param name string
---@return ffi.cdata*|nil cvar_t*
function CVar.find(name)
    local ptr = common.get_api().pfnGetCvarPointer(name)
    if ptr == nil or ptr == ffi.NULL then
        return nil
    end
    return ptr
end

--- Registers a new console variable
--- Memory for name and string value is anchored to prevent garbage collection
---@param name string
---@param defaultValue string|number
---@param flags? integer Flags from CVar.flags (default 0)
---@return ffi.cdata* cvar_t*
function CVar.register(name, defaultValue, flags)
    local c_name = ffi.new("char[?]", #name + 1)
    ffi.copy(c_name, name)

    local str_val = tostring(defaultValue or "")
    local c_val = ffi.new("char[?]", #str_val + 1)
    ffi.copy(c_val, str_val)

    common.anchor({ name = c_name, val = c_val })

    return common.get_api().pfnRegisterVariable(c_name, c_val, flags or 0)
end

--- Retrieves all registered engine cvars into a table
---@return table<string, {name: string, string: string, value: number, flags: integer}>
function CVar.getAll()
    local result = {}
    local curr = common.get_api().GetFirstCvarPtr()
    while curr ~= nil and curr ~= ffi.NULL do
        local name = common.c_str(curr.name)
        if name then
            result[name] = {
                name = name,
                string = common.c_str(curr.string) or "",
                value = tonumber(curr.value) or 0,
                flags = tonumber(curr.flags) or 0,
                ptr = curr
            }
        end
        curr = curr.next
    end
    return result
end

-- Metatable for syntactic sugar: cvar.fps_max, cvar.fps_max = 144
setmetatable(CVar, {
    __index = function(_, key)
        return CVar.getString(key)
    end,
    __newindex = function(_, key, val)
        CVar.set(key, val)
    end
})

return CVar
