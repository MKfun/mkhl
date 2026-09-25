local common = require("hltypes.engine.common")

---@class HLInput
local Input = {}

--- Retrieves current screen coordinates of mouse cursor
---@return integer x, integer y
function Input.getMousePos()
    local api = common.get_api()
    common.buf_int1[0] = 0
    common.buf_int2[0] = 0
    api.GetMousePosition(common.buf_int1, common.buf_int2)
    return common.buf_int1[0], common.buf_int2[0]
end

--- Sets mouse cursor coordinates
---@param x integer
---@param y integer
function Input.setMousePos(x, y)
    common.get_api().pfnSetMousePos(math.floor(x or 0), math.floor(y or 0))
end

--- Enables or disables mouse input handling
---@param enable boolean
function Input.setMouseEnable(enable)
    common.get_api().pfnSetMouseEnable(enable and 1 or 0)
end

--- Retrieves per-frame mouse movement delta (VGUI2)
---@return integer dx, integer dy
function Input.getMouseDelta()
    local api = common.get_api()
    common.buf_int1[0] = 0
    common.buf_int2[0] = 0
    api.pfnVguiWrap2_GetMouseDelta(common.buf_int1, common.buf_int2)
    return common.buf_int1[0], common.buf_int2[0]
end

--- Emulates key press or release event
---@param key integer Key code (from kbutton or ASCII)
---@param isDown boolean true for pressed, false for released
function Input.sendKeyEvent(key, isDown)
    common.get_api().Key_Event(key, isDown and 1 or 0)
end

--- Looks up key bound to specified command (e.g. "+jump")
---@param bindingCommand string
---@return string|nil
function Input.lookupBinding(bindingCommand)
    local ptr = common.get_api().Key_LookupBinding(tostring(bindingCommand))
    return common.c_str(ptr)
end

return Input
