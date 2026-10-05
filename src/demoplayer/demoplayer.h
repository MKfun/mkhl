#ifndef DEMOPLAYER_H
#define DEMOPLAYER_H

#include "demoplayer_ifaces.h"
#include "basesystemmodule.h"
#include "director_cmd.h"
#include "containers.h"
#include "bitbuffer.h"

class DemoPlayer : public IDemoPlayer, public BaseSystemModule, public IDirector
{
public:
	DemoPlayer();
	virtual ~DemoPlayer();

	// ISystemModule / IDemoPlayer shared methods
	virtual bool Init(IBaseSystem *system, int serial, char *name) override;
	virtual void RunFrame(double time) override;
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) override;
	virtual void ExecuteCommand(int cmd, char *str) override;
	virtual void RegisterListener(ISystemModule *listener) override;
	virtual void RemoveListener(ISystemModule *listener) override;
	virtual IBaseSystem *GetSystem() override;
	virtual int GetSerial() override;
	virtual char *GetStatusLine() override;
	virtual const char *GetType() override;
	virtual char *GetName() override;
	virtual int GetState() override;
	virtual float GetVersion() override;
	virtual void ShutDown() override;

	// IDirector / IDemoPlayer shared methods
	virtual bool NewGame(IWorld *world, IProxy *proxy) override;
	virtual char *GetModName() override;
	virtual int WriteCommands(BitBuffer *buf, float fromTime, float toTime) override;
	virtual bool AddCommand(DirectorCmd *cmd) override;
	virtual bool RemoveCommand(int index) override;
	virtual DirectorCmd *GetLastCommand() override;
	virtual IObjectContainer *GetCommands() override;

	// IDemoPlayer specific methods
	virtual void SetWorldTime(double time, bool relative) override;
	virtual void SetTimeScale(float scale) override;
	virtual void SetPaused(bool paused) override;
	virtual void SetEditMode(bool edit) override;
	virtual void SetMasterMode(bool master) override;
	virtual bool IsPaused() override;
	virtual bool IsLoading() override;
	virtual bool IsActive() override;
	virtual bool IsEditMode() override;
	virtual bool IsMasterMode() override;
	virtual void RemoveFrames(double startTime, double endTime) override;
	virtual bool ExecuteDirectorCmd(DirectorCmd *cmd) override;
	virtual double GetWorldTime() override;
	virtual double GetStartTime() override;
	virtual double GetEndTime() override;
	virtual float GetTimeScale() override;
	virtual IWorld *GetWorld() override;
	virtual char *GetFileName() override;
	virtual bool SaveGame(char *name) override;
	virtual bool LoadGame(char *name) override;
	virtual void Stop() override;
	virtual void ForceHLTV(bool force) override;
	virtual bool GetDemoViewInfo(ref_params_s *params, float *view, int *viewmodel) override;
	virtual int ReadDemoMessage(unsigned char *data, int size) override;
	virtual void ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged,
		int *incoming_reliable_acknowledged, int *incoming_reliable_sequence,
		int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence) override;

	// Internal helper methods
	void RunClocks();
	void WriteSpawn(BitBuffer *stream);
	void WriteDatagram(BitBuffer *stream);
	void WriteCameraPath(DirectorCmd *cmd, BitBuffer *stream);
	void ExecuteDemoFileCommands(BitBuffer *stream);

	void CMD_Jump(char *cmdLine);
	void CMD_ForceHLTV(char *cmdLine);
	void CMD_Pause(char *cmdLine);
	void CMD_Speed(char *cmdLine);
	void CMD_Start(char *cmdLine);
	void CMD_Save(char *cmdLine);

public:
	IEngineWrapper *m_Engine;
	IWorld *m_World;
	IServer *m_Server;
	ObjectDictionary m_Commands;
	DirectorCmd *m_LastCmd;
	int m_PlayerState;
	int m_Outgoing_sequence;
	char m_DemoFileName[4096];
	int m_LastFrameSeqNr;
	BitBuffer m_DemoStream;
	bool m_EditorMode;
	bool m_MasterMode;
	bool m_ForceHLTV;
	bool m_IsSaving;
	float m_TimeScale;
	double m_WorldTime;
	double m_PlayerTime;
	double m_StartTime;
	double m_LastFrameTime;
	bool m_IsPaused;
	double m_LastClockUpdateTime;
	int m_DeltaFrameSeqNr;
	int m_Unknown;
};

DemoPlayer *CreateDemoPlayer();

#endif // DEMOPLAYER_H
