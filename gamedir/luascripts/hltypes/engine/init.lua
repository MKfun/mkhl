local APIProxy = require("hltypes.APIProxy")

---@class HLEngine
local Engine = {
    console = require("hltypes.engine.console"),
    cvar    = require("hltypes.engine.cvar"),
    cmd     = require("hltypes.engine.cmd"),
    draw    = require("hltypes.engine.draw"),
    sprite  = require("hltypes.engine.sprite"),
    screen  = require("hltypes.engine.screen"),
    input   = require("hltypes.engine.input"),
    sound   = require("hltypes.engine.sound"),
    trace   = require("hltypes.engine.trace"),
    system  = require("hltypes.engine.system"),

    proxy   = APIProxy,
    getRaw  = APIProxy.get_engine,
    setRaw  = APIProxy.set_engine,
}

setmetatable(Engine, {
    __index = function(_, key)
        if key == "raw" then
            return APIProxy.get_engine()
        end
        return nil
    end
})

return Engine
