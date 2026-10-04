local common = require("hltypes.engine.common")

---@class HLConsole
local Console = {}

--- Prints text to the game console (automatically appends newline if missing)
--- Supports multiple arguments separated by spaces, similar to standard print()
---@param ... any
function Console.print(...)
    local n = select("#", ...)
    local parts = {}
    for i = 1, n do
        parts[i] = tostring(select(i, ...))
    end
    local msg = table.concat(parts, " ")
    if not msg:match("\n$") then
        msg = msg .. "\n"
    end
    common.get_api().pfnConsolePrint(msg)
end

--- Direct string print to the console without automatic '\n' appending
---@param str string
function Console.rawPrint(str)
    common.get_api().pfnConsolePrint(tostring(str))
end

--- Formatted print to the game console (via string.format)
---@param fmt string
---@param ... any
function Console.printf(fmt, ...)
    local msg = string.format(fmt, ...)
    if not msg:match("\n$") then
        msg = msg .. "\n"
    end
    common.get_api().pfnConsolePrint(msg)
end

--- Prints message to developer console (only shown when developer 1 is active)
---@param fmt string
---@param ... any
function Console.dprint(fmt, ...)
    local msg = string.format(fmt, ...)
    if not msg:match("\n$") then
        msg = msg .. "\n"
    end
    common.get_api().Con_DPrintf(msg)
end

--- Displays large text in the center of the screen
---@param text string
function Console.center(text)
    common.get_api().pfnCenterPrint(tostring(text))
end

--- Prints debug text at a fixed on-screen line index/slot
---@param pos integer Line position index on screen
---@param text string
function Console.nprint(pos, text)
    common.get_api().Con_NPrintf(pos, "%s", tostring(text))
end

--- Checks if the console window is currently visible
---@return boolean
function Console.isVisible()
    return common.get_api().Con_IsVisible() ~= 0
end

return Console
