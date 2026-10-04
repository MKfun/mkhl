local common = require("hltypes.engine.common")

---@class HLSound
local Sound = {}

--- Plays sound by filepath (e.g. "weapons/c4_beep5.wav")
---@param soundPath string
---@param volume? number Volume 0.0..1.0 (default 1.0)
function Sound.play(soundPath, volume)
    common.get_api().pfnPlaySoundByName(tostring(soundPath), volume or 1.0)
end

--- Plays pre-cached sound by its index
---@param soundIndex integer
---@param volume? number Volume 0.0..1.0 (default 1.0)
function Sound.playByIndex(soundIndex, volume)
    common.get_api().pfnPlaySoundByIndex(soundIndex, volume or 1.0)
end

--- Plays 3D positional sound at specified world coordinates
--- Accepts (x, y, z) coordinates or a table/array {x, y, z}
---@param soundPath string
---@param xOrPos number|table|number[]
---@param yOrVol? number
---@param z? number
---@param volume? number
function Sound.playAt(soundPath, xOrPos, yOrVol, z, volume)
    local api = common.get_api()
    local vol = 1.0
    if type(xOrPos) == "table" then
        common.buf_float3_1[0] = xOrPos[1] or xOrPos.x or 0
        common.buf_float3_1[1] = xOrPos[2] or xOrPos.y or 0
        common.buf_float3_1[2] = xOrPos[3] or xOrPos.z or 0
        vol = tonumber(yOrVol) or 1.0
    else
        common.buf_float3_1[0] = tonumber(xOrPos) or 0
        common.buf_float3_1[1] = tonumber(yOrVol) or 0
        common.buf_float3_1[2] = tonumber(z) or 0
        vol = tonumber(volume) or 1.0
    end
    api.pfnPlaySoundByNameAtLocation(tostring(soundPath), vol, common.buf_float3_1)
end

--- Plays sound with pitch shift
---@param soundPath string
---@param pitch? integer Pitch where 100 = 100% normal (e.g. 50 is low, 150 is high)
---@param volume? number Volume 0.0..1.0 (default 1.0)
function Sound.playPitch(soundPath, pitch, volume)
    common.get_api().pfnPlaySoundByNameAtPitch(tostring(soundPath), volume or 1.0, pitch or 100)
end

--- Plays voice sound with pitch shift
---@param soundPath string
---@param pitch? integer Pitch where 100 = 100% normal
---@param volume? number Volume 0.0..1.0 (default 1.0)
function Sound.playVoice(soundPath, pitch, volume)
    common.get_api().pfnPlaySoundVoiceByName(tostring(soundPath), volume or 1.0, pitch or 100)
end

--- Loads and streams a background music track (e.g. "media/gamestartup.mp3")
---@param musicPath string
---@param looping? boolean Loop playback
function Sound.playMusic(musicPath, looping)
    common.get_api().pfnPrimeMusicStream(tostring(musicPath), looping and 1 or 0)
end

--- Retrieves approximate playback length of a WAV sound file
---@param soundPath string
---@return integer
function Sound.getWaveLength(soundPath)
    return common.get_api().COM_GetApproxWavePlayLength(tostring(soundPath))
end

return Sound
