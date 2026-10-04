local ffi = require("ffi")

ffi.cdef[[

    enum {
        FCVAR_ARCHIVE          = (1 << 0),
        FCVAR_USERINFO         = (1 << 1),
        FCVAR_SERVER           = (1 << 2),
        FCVAR_EXTDLL           = (1 << 3),
        FCVAR_CLIENTDLL        = (1 << 4),
        FCVAR_PROTECTED        = (1 << 5),
        FCVAR_SPONLY           = (1 << 6),
        FCVAR_PRINTABLEONLY    = (1 << 7),
        FCVAR_UNLOGGED         = (1 << 8),
        FCVAR_NOEXTRAWHITEPACE = (1 << 9),
        FCVAR_PRIVILEGED       = (1 << 10),
        FCVAR_FILTERSTUFFTEXT  = (1 << 11),
        FCVAR_FILTERCHARS      = (1 << 12),
        FCVAR_NOBADPATHS       = (1 << 13),

        FCVAR_BHL_ARCHIVE      = (1 << 22),
        FCVAR_DEVELOPMENTONLY  = (1 << 23)
    };

    // Структура CVar
    typedef struct cvar_s {
        const char *name;
        const char *string;
        int flags;
        float value;
        struct cvar_s *next;
    } cvar_t;
]]


local FCVAR = {
    ARCHIVE          = 0x00000001,
    USERINFO         = 0x00000002,
    SERVER           = 0x00000004,
    EXTDLL           = 0x00000008,
    CLIENTDLL        = 0x00000010,
    PROTECTED        = 0x00000020,
    SPONLY           = 0x00000040,
    PRINTABLEONLY    = 0x00000080,
    UNLOGGED         = 0x00000100,
    NOEXTRAWHITEPACE = 0x00000200,
    PRIVILEGED       = 0x00000400,
    FILTERSTUFFTEXT  = 0x00000800,
    FILTERCHARS      = 0x00001000,
    NOBADPATHS       = 0x00002000,

    BHL_ARCHIVE      = 0x00400000,
    DEVELOPMENTONLY  = 0x00800000,
}

return {
    cvar_t = ffi.typeof("cvar_t"),
    flags  = FCVAR,


    hasFlag = function(cvar, flag)
    local bit = require("bit")
    return bit.band(cvar.flags, flag) ~= 0
    end
}
