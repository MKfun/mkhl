#ifndef DEMOPLAYER_IFACES_H
#define DEMOPLAYER_IFACES_H

#include "vinterface/interface.h"
#include "bitbuffer.h"
#include "containers.h"
#include "director_cmd.h"
#include "ref_params.h"

class IBaseSystem;
class ISystemModule;
class IWorld;
class IServer;
class IProxy;
class IEngineWrapper;
class IDirector;
class IDemoPlayer;

//=============================================================================
// ISystemModule
//=============================================================================
class ISystemModule : public virtual IBaseInterface
{
public:
	virtual ~ISystemModule() {}
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0;
	virtual void RunFrame(double time) = 0;
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0;
	virtual void ExecuteCommand(int cmd, char *str) = 0;
	virtual void RegisterListener(ISystemModule *listener) = 0;
	virtual void RemoveListener(ISystemModule *listener) = 0;
	virtual IBaseSystem *GetSystem() = 0;
	virtual int GetSerial() = 0;
	virtual char *GetStatusLine() = 0;
	virtual const char *GetType() = 0;
	virtual char *GetName() = 0;
	virtual int GetState() = 0;
	virtual float GetVersion() = 0;
	virtual void ShutDown() = 0;
	virtual char *COM_GetBaseDir() = 0;
};

//=============================================================================
// IDirector
//=============================================================================
class IDirector : public virtual ISystemModule
{
public:
	virtual ~IDirector() {}
	virtual bool NewGame(IWorld *world, IProxy *proxy) = 0;
	virtual char *GetModName() = 0;
	virtual int WriteCommands(BitBuffer *buf, float fromTime, float toTime) = 0;
	virtual bool AddCommand(DirectorCmd *cmd) = 0;
	virtual bool RemoveCommand(int index) = 0;
	virtual DirectorCmd *GetLastCommand() = 0;
	virtual IObjectContainer *GetCommands() = 0;
};

//=============================================================================
// IDemoPlayer
//=============================================================================
class IDemoPlayer : public IBaseInterface
{
public:
	virtual ~IDemoPlayer() {}

	// ISystemModule-like slots
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0;
	virtual void RunFrame(double time) = 0;
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0;
	virtual void ExecuteCommand(int cmd, char *str) = 0;
	virtual void RegisterListener(ISystemModule *listener) = 0;
	virtual void RemoveListener(ISystemModule *listener) = 0;
	virtual IBaseSystem *GetSystem() = 0;
	virtual int GetSerial() = 0;
	virtual char *GetStatusLine() = 0;
	virtual const char *GetType() = 0;
	virtual char *GetName() = 0;
	virtual int GetState() = 0;
	virtual float GetVersion() = 0;
	virtual void ShutDown() = 0;

	// IDirector-like slots
	virtual bool NewGame(IWorld *world, IProxy *proxy) = 0;
	virtual char *GetModName() = 0;
	virtual int WriteCommands(BitBuffer *buf, float fromTime, float toTime) = 0;
	virtual bool AddCommand(DirectorCmd *cmd) = 0;
	virtual bool RemoveCommand(int index) = 0;
	virtual DirectorCmd *GetLastCommand() = 0;
	virtual IObjectContainer *GetCommands() = 0;

	// IDemoPlayer specific slots
	virtual void SetWorldTime(double time, bool relative) = 0;
	virtual void SetTimeScale(float scale) = 0;
	virtual void SetPaused(bool paused) = 0;
	virtual void SetEditMode(bool edit) = 0;
	virtual void SetMasterMode(bool master) = 0;
	virtual bool IsPaused() = 0;
	virtual bool IsLoading() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsEditMode() = 0;
	virtual bool IsMasterMode() = 0;
	virtual void RemoveFrames(double startTime, double endTime) = 0;
	virtual bool ExecuteDirectorCmd(DirectorCmd *cmd) = 0;
	virtual double GetWorldTime() = 0;
	virtual double GetStartTime() = 0;
	virtual double GetEndTime() = 0;
	virtual float GetTimeScale() = 0;
	virtual IWorld *GetWorld() = 0;
	virtual char *GetFileName() = 0;
	virtual bool SaveGame(char *name) = 0;
	virtual bool LoadGame(char *name) = 0;
	virtual void Stop() = 0;
	virtual void ForceHLTV(bool force) = 0;
	virtual bool GetDemoViewInfo(ref_params_s *params, float *view, int *viewmodel) = 0;
	virtual int ReadDemoMessage(unsigned char *data, int size) = 0;
	virtual void ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged,
		int *incoming_reliable_acknowledged, int *incoming_reliable_sequence,
		int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence) = 0;
};

#define DEMOPLAYER_INTERFACE_VERSION "demoplayer001"

//=============================================================================
// External Interfaces
//=============================================================================

class IBaseSystem
{
public:
	virtual ~IBaseSystem() {}
	virtual void _pad0() = 0;
	virtual void _pad1() = 0;
	virtual void _pad2() = 0;
	virtual void _pad3() = 0;
	virtual void _pad4() = 0;
	virtual void _pad5() = 0;
	virtual void _pad6() = 0;
	virtual void _pad7() = 0;
	virtual void _pad8() = 0;
	virtual void _pad9() = 0;
	virtual void _pad10() = 0;
	virtual void _pad11() = 0;
	virtual void _pad12() = 0;
	virtual void _pad13() = 0;
	virtual void _pad14() = 0;
	virtual void _pad15() = 0;
	virtual void _pad16() = 0;
	virtual void Printf(const char *fmt, ...) = 0; // slot 19
	virtual void Print(const char *fmt, ...) = 0;  // slot 20
	virtual void _pad21() = 0;
	virtual void _pad22() = 0;
	virtual void _pad23() = 0;
	virtual void _pad24() = 0;
	virtual void _pad25() = 0;
	virtual void _pad26() = 0;
	virtual void _pad27() = 0;
	virtual void _pad28() = 0;
	virtual void _pad29() = 0;
	virtual void AddCommand(const char *name, ISystemModule *module, int cmdId) = 0; // slot 30
	virtual void _pad31() = 0;
	virtual void _pad32() = 0;
	virtual void _pad33() = 0;
	virtual void _pad34() = 0;
	virtual void _pad35() = 0;
	virtual bool AddModule(ISystemModule *module, const char *name = nullptr) = 0; // slot 36
	virtual void *GetModule(const char *interfacename, const char *library = "", const char *instancename = 0) = 0; // slot 37
	virtual bool RemoveModule(ISystemModule *module) = 0; // slot 38
};

class IEngineWrapper
{
public:
	virtual ~IEngineWrapper() {}
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0; // slot 2 (0x8)
	virtual void RunFrame(double time) = 0; // slot 3 (0xc)
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0; // slot 4 (0x10)
	virtual void ExecuteCommand(int cmd, char *str) = 0; // slot 5 (0x14)
	virtual void RegisterListener(ISystemModule *listener) = 0; // slot 6 (0x18)
	virtual void RemoveListener(ISystemModule *listener) = 0; // slot 7 (0x1c)
	virtual IBaseSystem *GetSystem() = 0; // slot 8 (0x20)
	virtual int GetSerial() = 0; // slot 9 (0x24)
	virtual char *GetStatusLine() = 0; // slot 10 (0x28)
	virtual const char *GetType() = 0; // slot 11 (0x2c)
	virtual char *GetName() = 0; // slot 12 (0x30)
	virtual int GetState() = 0; // slot 13 (0x34)
	virtual float GetVersion() = 0; // slot 14 (0x38)
	virtual void ShutDown() = 0; // slot 15 (0x3c)

	virtual void GetViewOrigin(float *origin) = 0; // slot 16 (0x40)
	virtual void GetViewAngles(float *angles) = 0; // slot 17 (0x44)
	virtual int GetTraceEntity() = 0; // slot 18 (0x48)
	virtual float GetCvarFloat(char *name) = 0; // slot 19 (0x4c)
	virtual const char *GetCvarString(char *name) = 0; // slot 20 (0x50)
	virtual void SetCvar(const char *name, const char *value) = 0; // slot 21 (0x54)
	virtual void Cbuf_AddText(const char *cmd) = 0; // slot 22 (0x58)
	virtual void DemoUpdateClientData(void *data) = 0; // slot 23 (0x5c)
	virtual void CL_QueueEvent(int flags, int index, float delay, void *args) = 0; // slot 24 (0x60)
	virtual void HudWeaponAnim(int iAnim, int body) = 0; // slot 25 (0x64)
	virtual void CL_DemoPlaySound(int sound, char *szSoundFile, float fvol, float attenuation, int fPitch, int fflags) = 0; // slot 26 (0x68)
	virtual void ClientDLL_ReadDemoBuffer(int size, unsigned char *buffer) = 0; // slot 27 (0x6c)
	virtual bool ValidStuffText(const char *cmd) = 0; // slot 28 (0x70)
	virtual void Cbuf_AddFilteredText(const char *cmd) = 0; // slot 29 (0x74)
};

class IServer
{
public:
	virtual ~IServer() {}
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0; // slot 2
	virtual void RunFrame(double time) = 0; // slot 3
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0; // slot 4
	virtual void ExecuteCommand(int cmd, char *str) = 0; // slot 5
	virtual void RegisterListener(ISystemModule *listener) = 0; // slot 6 (0x18)
	virtual void RemoveListener(ISystemModule *listener) = 0; // slot 7 (0x1c)
	virtual IBaseSystem *GetSystem() = 0; // slot 8 (0x20)
	virtual int GetSerial() = 0; // slot 9 (0x24)
	virtual char *GetStatusLine() = 0; // slot 10 (0x28)
	virtual const char *GetType() = 0; // slot 11 (0x2c)
	virtual char *GetName() = 0; // slot 12 (0x30)
	virtual int GetState() = 0; // slot 13 (0x34)
	virtual float GetVersion() = 0; // slot 14 (0x38)
	virtual void ShutDown() = 0; // slot 15 (0x3c)

	virtual bool Connect(IWorld *world, void *netaddress, void *netsocket) = 0; // slot 16 (0x40)
	virtual bool LoadDemo(IWorld *world, const char *name, bool forceHLTV, bool unk) = 0; // slot 17 (0x44)
	virtual void Reconnect() = 0; // slot 18 (0x48)
	virtual void Disconnect() = 0; // slot 19 (0x4c)
	virtual void Retry() = 0; // slot 20 (0x50)
	virtual void StopRetry() = 0; // slot 21 (0x54)
	virtual void SendStringCommand(const char *cmd) = 0; // slot 22 (0x58)
	virtual void SendHLTVCommand(BitBuffer *buf) = 0; // slot 23 (0x5c)
	virtual bool IsConnected() = 0; // slot 24 (0x60)
	virtual bool IsDemoFile() = 0; // slot 25 (0x64)
	virtual bool IsGameServer() = 0; // slot 26 (0x68)
	virtual bool IsRelayProxy() = 0; // slot 27 (0x6c)
	virtual bool IsVoiceBlocking() = 0; // slot 28 (0x70)
	virtual void SetProxy(void *proxy) = 0; // slot 29 (0x74)
	virtual void SetDirector(IDirector *director) = 0; // slot 30 (0x78)
	virtual void SetPlayerName(const char *name) = 0; // slot 31 (0x7c)
	virtual void SetDelayReconnect(bool delay) = 0; // slot 32 (0x80)
};

class IWorld
{
public:
	virtual ~IWorld() {}
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0; // slot 2
	virtual void RunFrame(double time) = 0; // slot 3
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0; // slot 4
	virtual void ExecuteCommand(int cmd, char *str) = 0; // slot 5
	virtual void RegisterListener(ISystemModule *listener) = 0; // slot 6 (0x18)
	virtual void RemoveListener(ISystemModule *listener) = 0; // slot 7 (0x1c)
	virtual IBaseSystem *GetSystem() = 0; // slot 8 (0x20)
	virtual int GetSerial() = 0; // slot 9 (0x24)
	virtual char *GetStatusLine() = 0; // slot 10 (0x28)
	virtual const char *GetType() = 0; // slot 11 (0x2c)
	virtual char *GetName() = 0; // slot 12 (0x30)
	virtual int GetState() = 0; // slot 13 (0x34)
	virtual float GetVersion() = 0; // slot 14 (0x38)
	virtual void ShutDown() = 0; // slot 15 (0x3c)

	virtual double GetTime() = 0; // slot 16 (0x40)
	virtual void *GetGameServerAddress() = 0; // slot 17 (0x44)
	virtual const char *GetLevelName() = 0; // slot 18 (0x48)
	virtual const char *GetGameDir() = 0; // slot 19 (0x4c)
	virtual void *GetFrameByTime(double time) = 0; // slot 20 (0x50)
	virtual void *GetFrameBySeqNr(unsigned int seq) = 0; // slot 21 (0x54)
	virtual void *GetLastFrame() = 0; // slot 22 (0x58)
	virtual void *GetFirstFrame() = 0; // slot 23 (0x5c)
	virtual int GetServerCount() = 0; // slot 24 (0x60)
	virtual int GetSlotNumber() = 0; // slot 25 (0x64)
	virtual int GetMaxClients() = 0; // slot 26 (0x68)
	virtual int GetNumPlayers() = 0; // slot 27 (0x6c)
	virtual void *GetWorldModel() = 0; // slot 28 (0x70)
	virtual const char *GetServerInfoString() = 0; // slot 29 (0x74)
	virtual void *GetPlayerInfoString(int idx, void *info) = 0; // slot 30 (0x78)
	virtual void *GetUserMsg(int msg) = 0; // slot 31 (0x7c)
	virtual const char *GetHostName() = 0; // slot 32 (0x80)
	virtual void *GetServerInfo() = 0; // slot 33 (0x84)
	virtual bool IsPlayerIndex(int idx) = 0; // slot 34 (0x88)
	virtual bool IsVoiceEnabled() = 0; // slot 35 (0x8c)
	virtual bool IsActive() = 0; // slot 36 (0x90)
	virtual bool IsPaused() = 0; // slot 37 (0x94)
	virtual bool IsComplete() = 0; // slot 38 (0x98)
	virtual bool IsHLTV() = 0; // slot 39 (0x9c)
	virtual void Reset() = 0; // slot 40 (0xa0)
	virtual void _pad41() = 0;
	virtual void _pad42() = 0;
	virtual void _pad43() = 0;
	virtual void _pad44() = 0;
	virtual void SetPaused(bool paused) = 0; // slot 45 (0xb4)
	virtual void SetTime(double time) = 0; // slot 46 (0xb8)
	virtual void SetBufferSize(float size) = 0; // slot 47 (0xbc)
	virtual void SetVoiceEnabled(bool voice) = 0; // slot 48 (0xc0)
	virtual void _pad49() = 0;
	virtual void _pad50() = 0;
	virtual void SetHLTV(bool hltv) = 0; // slot 51 (0xcc)
	virtual void _pad52() = 0;
	virtual void _pad53() = 0;
	virtual void _pad54() = 0;
	virtual void _pad55() = 0;
	virtual void _pad56() = 0;
	virtual void _pad57() = 0;
	virtual bool SaveAsDemo(const char *name, IDirector *director) = 0; // slot 58 (0xe8)
	virtual void StopGame() = 0; // slot 59 (0xec)
	virtual void _pad60() = 0;
	virtual void _pad61() = 0;
	virtual void _pad62() = 0;
	virtual void _pad63() = 0;
	virtual void _pad64() = 0;
	virtual void _pad65() = 0;
	virtual void _pad66() = 0;
	virtual void _pad67() = 0;
	virtual void _pad68() = 0;
	virtual void _pad69() = 0;
	virtual void _pad70() = 0;
	virtual void _pad71() = 0;
	virtual void _pad72() = 0;
	virtual void AddSignonData(unsigned char type, const void *data, int length) = 0; // slot 73 (0x124)
	virtual void _pad74() = 0;
	virtual void _pad75() = 0;
	virtual void _pad76() = 0;
	virtual void _pad77() = 0;
	virtual void WriteFrame(void *frame, unsigned int lastFrameSeqNr, BitBuffer *stream1, BitBuffer *stream2, unsigned int deltaFrameSeqNr, unsigned int outgoingSeq, bool unk) = 0; // slot 78 (0x138)
	virtual void WriteNewData(BitBuffer *stream) = 0; // slot 79 (0x13c)
	virtual void WriteClientUpdate(BitBuffer *stream, int client) = 0; // slot 80 (0x140)
	virtual void _pad81() = 0;
	virtual void WriteSigonData(BitBuffer *stream) = 0; // slot 82 (0x148)
	virtual void WriteLightStyles(BitBuffer *stream) = 0; // slot 83 (0x14c)
};

class IProxy
{
public:
	virtual ~IProxy() {}
};

#endif // DEMOPLAYER_IFACES_H

