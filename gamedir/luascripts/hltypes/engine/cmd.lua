local ffi = require("ffi")
local common = require("hltypes.engine.common")

---@class HLCmd
local Cmd = {}

--- Executes a command on the client (like in the console)
---@param cmdString string
function Cmd.client(cmdString)
    common.get_api().pfnClientCmd(tostring(cmdString))
end

--- Sends a command to the server reliably
---@param cmdString string
function Cmd.server(cmdString)
    common.get_api().pfnServerCmd(tostring(cmdString))
end

--- Sends a command to the server over unreliable channel
---@param cmdString string
function Cmd.serverUnreliable(cmdString)
    common.get_api().pfnServerCmdUnreliable(tostring(cmdString))
end

--- Executes a filtered command on the client
---@param cmdString string
---@return integer
function Cmd.filtered(cmdString)
    return common.get_api().pfnFilteredClientCmd(tostring(cmdString))
end

--- Returns argument count for the currently executed command
---@return integer
function Cmd.argc()
    return common.get_api().Cmd_Argc()
end

--- Retrieves command argument by index as string (0 is command name itself)
---@param index integer
---@return string
function Cmd.argv(index)
    local ptr = common.get_api().Cmd_Argv(index)
    return common.c_str(ptr) or ""
end

--- Retrieves all arguments of current command as a Lua table
--- Index 0 is command name, 1..N are arguments
---@return table<integer, string>
function Cmd.args()
    local count = Cmd.argc()
    local res = {}
    for i = 0, count - 1 do
        res[i] = Cmd.argv(i)
    end
    return res
end

--- Registers a new console command in the engine with error protection (pcall)
---@param name string Command name (e.g. "my_command")
---@param callback fun(args: table<integer, string>) Lua function invoked on command execution
function Cmd.add(name, callback)
    local c_name = ffi.new("char[?]", #name + 1)
    ffi.copy(c_name, name)

    local safe_callback = function()
        local current_args = Cmd.args()
        local ok, err = pcall(callback, current_args)
        if not ok then
            local api = common.get_api()
            api.pfnConsolePrint("^1[Lua Command Error: " .. name .. "]^7 " .. tostring(err) .. "\n")
        end
    end

    local c_callback = ffi.cast("CmdFunction", safe_callback)

    common.anchor({
        c_name = c_name,
        c_callback = c_callback,
        safe_callback = safe_callback
    })

    common.get_api().pfnAddCommand(c_name, c_callback)
end

--- Registers a keybind pair (+name and -name)
---@param name string Action name without + or - prefix
---@param onDown fun(args: table<integer, string>) Called when key is pressed down
---@param onUp? fun(args: table<integer, string>) Called when key is released
function Cmd.addBind(name, onDown, onUp)
    Cmd.add("+" .. name, onDown)
    if onUp then
        Cmd.add("-" .. name, onUp)
    else
        Cmd.add("-" .. name, function() end)
    end
end

--- Retrieves all aliases registered in the engine
---@return table<string, string>
function Cmd.getAliases()
    local result = {}
    local curr = common.get_api().pfnGetAliasList()
    while curr ~= nil and curr ~= ffi.NULL do
        local alias_name = common.c_str(curr.name)
        if alias_name then
            result[alias_name] = common.c_str(curr.value) or ""
        end
        curr = curr.next
    end
    return result
end

--- Retrieves a list of registered engine command names
---@return string[]
function Cmd.getAllCommands()
    local result = {}
    local api = common.get_api()
    local h = api.GetFirstCmdFunctionHandle()
    while h ~= nil and h ~= ffi.NULL do
        local name_ptr = api.GetCmdFunctionName(h)
        local name = common.c_str(name_ptr)
        if name then
            table.insert(result, name)
        end
        h = api.GetNextCmdFunctionHandle(h)
    end
    return result
end

return Cmd
