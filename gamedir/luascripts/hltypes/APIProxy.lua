local ffi = require("ffi")

ffi.cdef[[

    typedef int qboolean;
    typedef unsigned char byte;
    typedef int int32;
    typedef int HSPRITE;

    enum {
        MAX_ALIAS_NAME = 32
    };

    typedef struct wrect_s {
        int left;
        int right;
        int top;
        int bottom;
    } wrect_t;

    struct cl_entity_s;
    typedef struct cl_entity_s cl_entity_t;

    struct TEMPENTITY;
    typedef struct TEMPENTITY TEMPENTITY;

    struct event_args_t;
    typedef struct event_args_t event_args_t;

    struct client_data_t;
    typedef struct client_data_t client_data_t;

    struct playermove_s;
    struct usercmd_s;
    struct kbutton_s;
    struct ref_params_s;
    struct mstudioevent_s;
    struct local_state_s;
    struct entity_state_s;
    struct clientdata_s;
    struct weapon_data_s;
    struct netadr_s;
    struct tempent_s;
    struct r_studio_interface_s;
    struct engine_studio_api_s;

    struct client_sprite_t;
    typedef struct client_sprite_t client_sprite_t;

    typedef struct SCREENINFO {
        int iSize;
        int iWidth;
        int iHeight;
        int iFlags;
        int iCharHeight;
        short charWidths[256];
    } SCREENINFO;

    struct cvar_t;
    typedef struct cvar_t cvar_t;

    struct cmd_function_t;
    typedef struct cmd_function_t cmd_function_t;

    typedef struct hud_player_info_t {
        char *name;
        short ping;
        byte thisplayer;
        byte spectator;
        byte packetloss;
        char *model;
        short topcolor;
        short bottomcolor;
    } hud_player_info_t;

    struct client_textmessage_t;
    typedef struct client_textmessage_t client_textmessage_t;

    struct con_nprint_s;

    typedef struct pmplane_s {
        float normal[3];
        float dist;
    } pmplane_t;

    typedef struct pmtrace_s {
        qboolean allsolid;
        qboolean startsolid;
        qboolean inopen;
        qboolean inwater;
        float fraction;
        float endpos[3];
        pmplane_t plane;
        int ent;
        float deltavelocity[3];
        int hitgroup;
    } pmtrace_t;

    struct model_s;

    struct edict_t;
    typedef struct edict_t edict_t;

    typedef struct tagPOINT {
        int x;
        int y;
    } POINT;

    typedef struct screenfade_s {
        float fadeSpeed;
        float fadeEnd;
        float fadeTotalEnd;
        float fadeReset;
        byte fader, fadeg, fadeb, fadealpha;
        int fadeFlags;
    } screenfade_t;

    struct sequenceEntry_s;
    typedef struct sequenceEntry_s sequenceEntry_s;

    struct sentenceEntry_s;
    typedef struct sentenceEntry_s sentenceEntry_s;

    struct triangleapi_s;
    struct efx_api_s;
    struct event_api_s;
    struct demo_api_s;
    struct net_api_s;
    struct IVoiceTweak_s;

    struct cl_enginefunc_t;

    typedef void (*CmdFunction)(void);
    typedef int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity);
    typedef void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp);
    typedef int (*UserMsgHookFn)(const char *pszName, int iSize, void *pbuf);
    typedef void (*EventHookFn)(event_args_t *args);

    typedef struct cmdalias_s {
        struct cmdalias_s *next;
        char name[MAX_ALIAS_NAME];
        char *value;
    } cmdalias_t;

    typedef struct cldll_func_t {
        int (*pInitFunc)(struct cl_enginefunc_t *pEngineFuncs, int iVersion);
        void (*pHudInitFunc)(void);
        int (*pHudVidInitFunc)(void);
        int (*pHudRedrawFunc)(float flCurrentTime, int bIsIntermission);
        int (*pHudUpdateClientDataFunc)(client_data_t *pCLData, float flCurrentTime);
        void (*pHudResetFunc)(void);
        void (*pClientMove)(struct playermove_s *ppmove, qboolean server);
        void (*pClientMoveInit)(struct playermove_s *ppmove);
        char (*pClientTextureType)(char *pszName);
        void (*pIN_ActivateMouse)(void);
        void (*pIN_DeactivateMouse)(void);
        void (*pIN_MouseEvent)(int mstate);
        void (*pIN_ClearStates)(void);
        void (*pIN_Accumulate)(void);
        void (*pCL_CreateMove)(float frametime, struct usercmd_s *cmd, int bActive);
        int (*pCL_IsThirdPerson)(void);
        void (*pCL_GetCameraOffsets)(float *ofs);
        struct kbutton_s *(*pFindKey)(const char *pszName);
        void (*pCamThink)(void);
        void (*pCalcRefdef)(struct ref_params_s *pparams);
        int (*pAddEntity)(int type, struct cl_entity_s *ent, const char *pszModelName);
        void (*pCreateEntities)(void);
        void (*pDrawNormalTriangles)(void);
        void (*pDrawTransparentTriangles)(void);
        void (*pStudioEvent)(const struct mstudioevent_s *event, const struct cl_entity_s *entity);
        void (*pPostRunCmd)(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
        void (*pShutdown)(void);
        void (*pTxferLocalOverrides)(struct entity_state_s *state, const struct clientdata_s *client);
        void (*pProcessPlayerState)(struct entity_state_s *dst, const struct entity_state_s *src);
        void (*pTxferPredictionData)(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
        void (*pReadDemoBuffer)(int size, unsigned char *buffer);
        int (*pConnectionlessPacket)(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
        int (*pGetHullBounds)(int hullnumber, float *mins, float *maxs);
        void (*pHudFrame)(const double flFrameTime);
        int (*pKeyEvent)(const int bDown, int keynum, const char *pszCurrentBinding);
        void (*pTempEntUpdate)(const double flFrameTime, const double flClientTime, const double flCLGravity,
            struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive,
            Callback_AddVisibleEntity pAddVisibleEnt, Callback_TempEntPlaySound pTempPlaySound);
        cl_entity_t *(*pGetUserEntity)(int index);
        void (*pVoiceStatus)(int entindex, qboolean bTalking);
        void (*pDirectorMessage)(int iSize, void *pbuf);
        int (*pStudioInterface)(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
        void (*pChatInputPosition)(int *x, int *y);
        int (*pGetPlayerTeam)(int iplayer);
        void *(*pClientFactory)(void);
    } cldll_func_t;

    typedef struct cl_enginefunc_t {
        HSPRITE (*pfnSPR_Load)(const char *szPicName);
        int (*pfnSPR_Frames)(HSPRITE hPic);
        int (*pfnSPR_Height)(HSPRITE hPic, int frame);
        int (*pfnSPR_Width)(HSPRITE hPic, int frame);
        void (*pfnSPR_Set)(HSPRITE hPic, int r, int g, int b);
        void (*pfnSPR_Draw)(int frame, int x, int y, const wrect_t *prc);
        void (*pfnSPR_DrawHoles)(int frame, int x, int y, const wrect_t *prc);
        void (*pfnSPR_DrawAdditive)(int frame, int x, int y, const wrect_t *prc);
        void (*pfnSPR_EnableScissor)(int x, int y, int width, int height);
        void (*pfnSPR_DisableScissor)(void);
        client_sprite_t *(*pfnSPR_GetList)(const char *const pszName, int *piCount);
        void (*pfnFillRGBA)(int x, int y, int width, int height, int r, int g, int b, int a);
        int (*pfnGetScreenInfo)(SCREENINFO *pscrinfo);
        void (*pfnSetCrosshair)(HSPRITE hspr, wrect_t rc, int r, int g, int b);
        cvar_t *(*pfnRegisterVariable)(const char *const pszName, const char *const pszValue, int flags);
        float (*pfnGetCvarFloat)(const char *const pszName);
        const char *(*pfnGetCvarString)(const char *const pszName);
        int (*pfnAddCommand)(const char *const pszCmdName, CmdFunction pCallback);
        int (*pfnHookUserMsg)(const char *const pszMsgName, UserMsgHookFn pfn);
        int (*pfnServerCmd)(const char *const pszCmdString);
        int (*pfnClientCmd)(const char *const pszCmdString);
        void (*pfnGetPlayerInfo)(int ent_num, hud_player_info_t *pinfo);
        void (*pfnPlaySoundByName)(const char *const pszSound, float volume);
        void (*pfnPlaySoundByIndex)(int iSound, float volume);
        void (*pfnAngleVectors)(const float *vecAngles, float *forward, float *right, float *up);
        client_textmessage_t *(*pfnTextMessageGet)(const char *const pszName);
        int (*pfnDrawCharacter)(int x, int y, int number, int r, int g, int b);
        int (*pfnDrawConsoleString)(int x, int y, const char *const pszString);
        void (*pfnDrawSetTextColor)(float r, float g, float b);
        void (*pfnDrawConsoleStringLen)(const char *const pszString, int *piLength, int *piHeight);
        void (*pfnConsolePrint)(const char *const pszString);
        void (*pfnCenterPrint)(const char *const pszString);
        int (*GetWindowCenterX)(void);
        int (*GetWindowCenterY)(void);
        void (*GetViewAngles)(float *vecAngles);
        void (*SetViewAngles)(const float *vecAngles);
        int (*GetMaxClients)(void);
        void (*Cvar_SetValue)(const char *const pszCVarName, float value);
        int (*Cmd_Argc)(void);
        char *(*Cmd_Argv)(int arg);
        void (*Con_Printf)(const char *const pszFormat, ...);
        void (*Con_DPrintf)(const char *const pszFormat, ...);
        void (*Con_NPrintf)(const int pos, const char *const pszFormat, ...);
        void (*Con_NXPrintf)(struct con_nprint_s *info, char *fmt, ...);
        const char *(*PhysInfo_ValueForKey)(const char *const pszKey);
        const char *(*ServerInfo_ValueForKey)(const char *const pszKey);
        float (*GetClientMaxspeed)(void);
        int (*CheckParm)(const char *const pszParm, char **ppszNext);
        void (*Key_Event)(int key, const int bDown);
        void (*GetMousePosition)(int *mx, int *my);
        int (*IsNoClipping)(void);
        cl_entity_t *(*GetLocalPlayer)(void);
        cl_entity_t *(*GetViewModel)(void);
        cl_entity_t *(*GetEntityByIndex)(int idx);
        float (*GetClientTime)(void);
        void (*V_CalcShake)(void);
        void (*V_ApplyShake)(float *vecOrigin, float *vecAngles, const float flFactor);
        int (*PM_PointContents)(const float *vecPoint, int *piTruecontents);
        int (*PM_WaterEntity)(const float *vecPosition);
        struct pmtrace_s *(*PM_TraceLine)(const float *vecStart, const float *vecEnd, int flags, int usehull, int ignore_pe);
        struct model_s *(*CL_LoadModel)(const char *const pszModelName, int *piIndex);
        int (*CL_CreateVisibleEntity)(int type, cl_entity_t *ent);
        const struct model_s *(*GetSpritePointer)(HSPRITE hSprite);
        void (*pfnPlaySoundByNameAtLocation)(const char *const pszSoundName, float volume, const float *vecOrigin);
        unsigned short (*pfnPrecacheEvent)(int type, const char *const pszName);
        void (*pfnPlaybackEvent)(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
            const float *origin, const float *angles,
            float fparam1, float fparam2,
            int iparam1, int iparam2,
            int bparam1, int bparam2);
        void (*pfnWeaponAnim)(int iAnim, int body);
        float (*pfnRandomFloat)(float flLow, float flHigh);
        int32 (*pfnRandomLong)(int32 lLow, int32 lHigh);
        void (*pfnHookEvent)(const char *const pszName, EventHookFn pEventHook);
        int (*Con_IsVisible)(void);
        const char *(*pfnGetGameDirectory)(void);
        cvar_t *(*pfnGetCvarPointer)(const char *const pszName);
        const char *(*Key_LookupBinding)(const char *const pszBinding);
        const char *(*pfnGetLevelName)(void);
        void (*pfnGetScreenFade)(struct screenfade_s **);
        void (*pfnSetScreenFade)(struct screenfade_s **);
        void *(*VGui_GetPanel)(void);
        void (*VGui_ViewportPaintBackground)(int extents[4]);
        byte *(*COM_LoadFile)(const char *pszPath, int usehunk, int *piLength);
        char *(*COM_ParseFile)(char *pszData, char *pszToken);
        void (*COM_FreeFile)(void *pBuffer);
        struct triangleapi_s *pTriAPI;
        struct efx_api_s *pEfxAPI;
        struct event_api_s *pEventAPI;
        struct demo_api_s *pDemoAPI;
        struct net_api_s *pNetAPI;
        struct IVoiceTweak_s *pVoiceTweak;
        int (*IsSpectateOnly)(void);
        struct model_s *(*LoadMapSprite)(const char *pszFileName);
        void (*COM_AddAppDirectoryToSearchPath)(const char *const pszBaseDir, const char *const pszAppName);
        int (*COM_ExpandFilename)(const char *const pszFileName, char *pszNameOutBuffer, int nameOutBufferSize);
        const char *(*PlayerInfo_ValueForKey)(int playerNum, const char *key);
        void (*PlayerInfo_SetValueForKey)(const char *const pszKey, const char *const pszValue);
        qboolean (*GetPlayerUniqueID)(int iPlayer, char playerID[16]);
        int (*GetTrackerIDForPlayer)(int playerSlot);
        int (*GetPlayerForTrackerID)(int trackerID);
        int (*pfnServerCmdUnreliable)(const char *const pszCmdString);
        void (*pfnGetMousePos)(struct tagPOINT *ppt);
        void (*pfnSetMousePos)(int x, int y);
        void (*pfnSetMouseEnable)(qboolean fEnable);
        cvar_t *(*GetFirstCvarPtr)(void);
        cmd_function_t *(*GetFirstCmdFunctionHandle)(void);
        cmd_function_t *(*GetNextCmdFunctionHandle)(cmd_function_t *cmdhandle);
        const char *(*GetCmdFunctionName)(cmd_function_t *cmdhandle);
        float (*hudGetClientOldTime)(void);
        float (*hudGetServerGravityValue)(void);
        struct model_s *(*hudGetModelByIndex)(int index);
        void (*pfnSetFilterMode)(int bMode);
        void (*pfnSetFilterColor)(float r, float g, float b);
        void (*pfnSetFilterBrightness)(float brightness);
        sequenceEntry_s *(*pfnSequenceGet)(const char *const pszFileName, const char *const pszEntryName);
        void (*pfnSPR_DrawGeneric)(int frame, int x, int y, const wrect_t *prc, int src, int dest, int w, int h);
        sentenceEntry_s *(*pfnSequencePickSentence)(const char *const pszGroupName, int pickMethod, int *piPicked);
        int (*pfnDrawString)(int x, int y, const char *const pszString, int r, int g, int b);
        int (*pfnDrawStringReverse)(int x, int y, const char *const pszString, int r, int g, int b);
        const char *(*LocalPlayerInfo_ValueForKey)(const char *const pszKey);
        int (*pfnVGUI2DrawCharacter)(int x, int y, int ch, unsigned int font);
        int (*pfnVGUI2DrawCharacterAdd)(int x, int y, int ch, int r, int g, int b, unsigned int font);
        unsigned int (*COM_GetApproxWavePlayLength)(const char *const pszFileName);
        void *(*pfnGetCareerUI)(void);
        void (*Cvar_Set)(const char *const pszCVarName, const char *const pszValue);
        int (*pfnIsCareerMatch)(void);
        void (*pfnPlaySoundVoiceByName)(const char *const pszSoundName, float volume, int pitch);
        void (*pfnPrimeMusicStream)(const char *const pszFileName, const int bLooping);
        double (*GetAbsoluteTime)(void);
        void (*pfnProcessTutorMessageDecayBuffer)(int *pBuffer, int bufferLength);
        void (*pfnConstructTutorMessageDecayBuffer)(int *pBuffer, int bufferLength);
        void (*pfnResetTutorMessageDecayData)(void);
        void (*pfnPlaySoundByNameAtPitch)(const char *const pszSoundName, float volume, int pitch);
        void (*pfnFillRGBABlend)(int x, int y, int width, int height, int r, int g, int b, int a);
        int (*pfnGetAppID)(void);
        cmdalias_t *(*pfnGetAliasList)(void);
        void (*pfnVguiWrap2_GetMouseDelta)(int *x, int *y);
        int (*pfnFilteredClientCmd)(const char *szCmdString);
    } cl_enginefunc_t;

    cl_enginefunc_t* GetEngineAPI(void);

]]

local current_engine = (function()
    local ok, api = pcall(function() return ffi.C.GetEngineAPI() end)
    return ok and api or nil
end)()

local M
M = {
    cldll_func_t = ffi.typeof("cldll_func_t"),
    cl_enginefunc_t = ffi.typeof("cl_enginefunc_t"),
    cmdalias_t = ffi.typeof("cmdalias_t"),
    engfuncs = current_engine,
    bind_engine = function(ptr)
        return ffi.cast("cl_enginefunc_t*", ptr)
    end,
    bind_client = function(ptr)
        return ffi.cast("cldll_func_t*", ptr)
    end,
    get_engine = function()
        return M.engfuncs
    end,
    set_engine = function(ptr)
        M.engfuncs = ffi.cast("cl_enginefunc_t*", ptr)
        return M.engfuncs
    end
}

return M
