#include "demoplayer.h"
#include "tokenline.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cstdarg>

static void NormalizeAngles(float *angles)
{
	for (int i = 0; i < 3; i++)
	{
		if (angles[i] > 180.0f)
			angles[i] -= 360.0f;
		else if (angles[i] < -180.0f)
			angles[i] += 360.0f;
	}
}

static inline float LittleFloat(float val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	union
	{
		float f;
		uint32_t i;
	} u;
	u.f = val;
	u.i = __builtin_bswap32(u.i);
	return u.f;
#else
	return val;
#endif
}

static inline int32_t LittleLong(int32_t val)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return __builtin_bswap32(val);
#else
	return val;
#endif
}

struct frame_t
{
	float time;
	unsigned int seqNr;
};

DemoPlayer::DemoPlayer()
{
	m_Engine = nullptr;
	m_World = nullptr;
	m_Server = nullptr;
	m_LastCmd = nullptr;
	m_PlayerState = 0;
	m_Outgoing_sequence = 0;
	m_LastFrameSeqNr = 0;
	m_DeltaFrameSeqNr = 0;
	m_EditorMode = false;
	m_MasterMode = false;
	m_ForceHLTV = false;
	m_IsSaving = false;
	m_IsPaused = false;
	m_TimeScale = 1.0f;
	m_WorldTime = 0.0;
	m_PlayerTime = 0.0;
	m_StartTime = 0.0;
	m_LastFrameTime = 0.0;
	m_LastClockUpdateTime = 0.0;
	m_Unknown = 0;
	memset(m_DemoFileName, 0, sizeof(m_DemoFileName));
}

DemoPlayer::~DemoPlayer()
{
	m_DemoStream.Free();
	m_Commands.Clear(true);
}

bool DemoPlayer::Init(IBaseSystem *system, int serial, char *name)
{
	if (!BaseSystemModule::Init(system, serial, name))
		return false;

	if (name)
	{
		strncpy(m_Name, name, 254);
		m_Name[254] = '\0';
	}
	else
	{
		snprintf(m_Name, 255, "demoplayer001");
	}

	m_Engine = (IEngineWrapper *)m_System->GetModule("enginewrapper002", "", NULL);
	if (!m_Engine && m_System)
	{
		// SO STUPID DLLHACKHACKHACK: In HL25 build GCC optimized out
		// the call to AddModule(m_pEngineWrapper, "enginewrapper002")!
		// However, m_pEngineWrapper was allocated and saved in SystemWrapper
		// at offset 0x17c. VOLVO PLS FIX THIS!!!!

		m_Engine = *(IEngineWrapper **)((char *)m_System + 0x17c);
		if (m_Engine)
		{
			m_System->AddModule((ISystemModule *)m_Engine, "enginewrapper002");
		}
	}
	if (!m_Engine)
	{
		m_System->Printf("DemoPlayer::Init: couldn't get engine interface.\n");
		return false;
	}

	m_Server = (IServer *)m_System->GetModule("server001", "core", "DemoServer");
	if (!m_Server)
	{
		m_System->Printf("DemoPlayer::Init: couldn't load server module.\n");
		return false;
	}

	m_Server->RegisterListener(this);
	m_Server->SetDirector((IDirector *)this);
	m_Server->SetDelayReconnect(false);

	m_World = (IWorld *)m_System->GetModule("world001", "core", "DemoWorld");
	if (!m_World)
	{
		m_System->Printf("DemoPlayer::Init: couldn't load world module.\n");
		return false;
	}

	m_System->AddCommand("dem_jump", this, 1);
	m_System->AddCommand("dem_forcehltv", this, 2);
	m_System->AddCommand("dem_pause", this, 3);
	m_System->AddCommand("dem_speed", this, 4);
	m_System->AddCommand("dem_start", this, 5);
	m_System->AddCommand("dem_save", this, 6);

	m_World->RegisterListener(this);
	m_DemoStream.Resize(65536);
	SetTimeScale(1.0f);
	SetPaused(false);

	m_PlayerState = 0;
	m_Outgoing_sequence = 0;
	m_LastFrameSeqNr = 0;
	m_DeltaFrameSeqNr = 0;
	m_Unknown = 0;
	m_LastFrameTime = 0.0;
	m_EditorMode = false;
	m_MasterMode = true;
	m_ForceHLTV = false;
	m_IsSaving = false;
	memset(m_DemoFileName, 0, sizeof(m_DemoFileName));

	m_State = 3;
	m_System->Print("DemoPlayer module initialized.\n");
	return true;
}

void DemoPlayer::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);
	if (m_PlayerState != 0)
		RunClocks();
}

void DemoPlayer::RunClocks()
{
	float dt = (float)(m_SystemTime - m_LastClockUpdateTime);
	m_LastClockUpdateTime = m_SystemTime;
	m_PlayerTime += dt;

	if (m_World && m_World->IsActive() && !m_IsPaused)
	{
		m_WorldTime += dt;

		float *lastFrame = (float *)m_World->GetFrameBySeqNr(m_LastFrameSeqNr);
		float *nextFrame = (float *)m_World->GetFrameBySeqNr(m_LastFrameSeqNr + 1);
		if (nextFrame && lastFrame)
		{
			if (*nextFrame - *lastFrame > 2.0f)
				m_WorldTime = (double)(*nextFrame - 0.01f);
		}

		float *firstFramePtr = (float *)m_World->GetFirstFrame();
		float *lastFramePtr = (float *)m_World->GetLastFrame();
		if (firstFramePtr && lastFramePtr)
		{
			if (m_WorldTime > (double)*lastFramePtr)
			{
				m_WorldTime = (double)*lastFramePtr;
			}
			else if ((double)*firstFramePtr > m_WorldTime)
			{
				m_WorldTime = (double)(*firstFramePtr - 0.01f);
			}
		}
	}
}

void DemoPlayer::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	(void)data;
	if (!module)
		return;

	int senderSerial = module->GetSerial();
	if (m_Server && senderSerial == m_Server->GetSerial())
	{
		if (signal == 6 && m_System)
		{
			m_System->Printf("Demo file completely loaded.\n");
		}
	}
	else if (m_World && senderSerial == m_World->GetSerial())
	{
		BitBuffer v6(32);
		if (signal == 2)
		{
			NewGame(m_World, nullptr);
		}
		else if (signal > 2)
		{
			if (signal == 5 || signal == 6)
			{
				v6.WriteByte(24);
				v6.WriteByte(signal == 5 ? 1 : 0);
			}
		}
		else if (signal == 1)
		{
			m_StartTime = 9999.0;
		}
	}
}

void DemoPlayer::ExecuteCommand(int cmd, char *str)
{
	switch (cmd)
	{
	case 1:
		CMD_Jump(str);
		break;
	case 2:
		CMD_ForceHLTV(str);
		break;
	case 3:
		CMD_Pause(str);
		break;
	case 4:
		CMD_Speed(str);
		break;
	case 5:
		CMD_Start(str);
		break;
	case 6:
		CMD_Save(str);
		break;
	default:
		BaseSystemModule::ExecuteCommand(cmd, str);
		break;
	}
}

void DemoPlayer::RegisterListener(ISystemModule *listener)
{
	BaseSystemModule::RegisterListener(listener);
}

void DemoPlayer::RemoveListener(ISystemModule *listener)
{
	BaseSystemModule::RemoveListener(listener);
}

IBaseSystem *DemoPlayer::GetSystem()
{
	return BaseSystemModule::GetSystem();
}

int DemoPlayer::GetSerial()
{
	return BaseSystemModule::GetSerial();
}

char *DemoPlayer::GetStatusLine()
{
	return (char *)"No status available.\n";
}

const char *DemoPlayer::GetType()
{
	return DEMOPLAYER_INTERFACE_VERSION;
}

char *DemoPlayer::GetName()
{
	return BaseSystemModule::GetName();
}

int DemoPlayer::GetState()
{
	return BaseSystemModule::GetState();
}

float DemoPlayer::GetVersion()
{
	return BaseSystemModule::GetVersion();
}

void DemoPlayer::ShutDown()
{
	if (m_State != 4)
	{
		if (m_World)
			m_World->ShutDown();
		if (m_Server)
			m_Server->ShutDown();

		m_DemoStream.Free();
		m_Commands.Clear(true);
		BaseSystemModule::ShutDown();

		if (m_System)
			m_System->Print("DemoPlayer module Shutdown.\n");
	}
}

bool DemoPlayer::NewGame(IWorld *world, IProxy *proxy)
{
	(void)proxy;
	m_World = world;
	m_PlayerTime = 1.0;
	m_PlayerState = 2;
	m_Commands.Clear(true);
	m_LastCmd = nullptr;
	BaseSystemModule::FireSignal(1, nullptr);

	if (m_World && (m_World->IsHLTV() || m_ForceHLTV))
	{
		static unsigned char cmd[2] = { 1, 1 };
		m_World->AddSignonData(51, cmd, 2);
	}
	return true;
}

char *DemoPlayer::GetModName()
{
	return (char *)"valve";
}

int DemoPlayer::WriteCommands(BitBuffer *buf, float fromTime, float toTime)
{
	DirectorCmd *cmd = (DirectorCmd *)m_Commands.FindClosestKey(fromTime);
	while (cmd)
	{
		if (cmd->m_Time > toTime)
			break;

		if (cmd->m_Time <= fromTime)
		{
			cmd = (DirectorCmd *)m_Commands.GetNext();
			continue;
		}

		if (!m_IsSaving)
		{
			if (cmd->m_Type == 5)
			{
				float timescale = 1.0f;
				cmd->GetTimeScaleData(timescale);
				buf->WriteByte(55);
				buf->WriteFloat(timescale);
				m_TimeScale = timescale;
			}
			else if (cmd->m_Type == 14)
			{
				float v[3];
				float fov;
				int flags;
				cmd->GetCamPathData(v, v, fov, flags);
				if (flags & 1)
					WriteCameraPath(cmd, buf);
			}
			else
			{
				cmd->WriteToStream(buf);
			}

			m_LastCmd = cmd;
			BaseSystemModule::FireSignal(2, &cmd->m_Index);
			if (m_System)
			{
				m_System->Print("Director Cmd %s, Time %.2f\n", cmd->GetName(), cmd->GetTime());
			}
		}
		else
		{
			cmd->WriteToStream(buf);
		}

		cmd = (DirectorCmd *)m_Commands.GetNext();
	}
	return 0;
}

bool DemoPlayer::AddCommand(DirectorCmd *cmd)
{
	if (!cmd || cmd->GetType() == 15)
		return false;

	DirectorCmd *newCmd = new DirectorCmd();
	newCmd->Copy(cmd);
	float key = newCmd->GetTime();

	if (m_Commands.Add(newCmd, key))
	{
		int idx = 1;
		for (DirectorCmd *p = (DirectorCmd *)m_Commands.GetFirst(); p; p = (DirectorCmd *)m_Commands.GetNext())
		{
			p->m_Index = idx++;
		}
		BaseSystemModule::FireSignal(1, nullptr);
		return (bool)newCmd->m_Index;
	}
	else
	{
		delete newCmd;
		return false;
	}
}

bool DemoPlayer::RemoveCommand(int index)
{
	DirectorCmd *target = nullptr;
	for (DirectorCmd *cmd = (DirectorCmd *)m_Commands.GetFirst(); cmd; cmd = (DirectorCmd *)m_Commands.GetNext())
	{
		if (cmd->m_Index == index)
		{
			target = cmd;
			break;
		}
	}

	if (!target)
		return false;

	m_Commands.Remove(target);
	if (m_LastCmd == target)
		m_LastCmd = nullptr;

	delete target;

	int idx = 1;
	for (DirectorCmd *p = (DirectorCmd *)m_Commands.GetFirst(); p; p = (DirectorCmd *)m_Commands.GetNext())
	{
		p->m_Index = idx++;
	}

	BaseSystemModule::FireSignal(1, nullptr);
	return true;
}

DirectorCmd *DemoPlayer::GetLastCommand()
{
	return m_LastCmd;
}

IObjectContainer *DemoPlayer::GetCommands()
{
	return &m_Commands;
}

void DemoPlayer::SetWorldTime(double time, bool relative)
{
	if (relative)
		m_WorldTime += time;
	else
		m_WorldTime = time;
}

void DemoPlayer::SetTimeScale(float scale)
{
	m_TimeScale = scale;
	if (m_TimeScale > 4.0f)
		m_TimeScale = 4.0f;
	else if (m_TimeScale < 0.05f)
		m_TimeScale = 0.05f;

	m_DemoStream.WriteByte(55);
	m_DemoStream.WriteFloat(m_TimeScale);
}

void DemoPlayer::SetPaused(bool paused)
{
	m_IsPaused = paused;
}

void DemoPlayer::SetEditMode(bool edit)
{
	m_EditorMode = edit;
}

void DemoPlayer::SetMasterMode(bool master)
{
	m_MasterMode = master;
}

bool DemoPlayer::IsPaused()
{
	return m_IsPaused;
}

bool DemoPlayer::IsLoading()
{
	if (m_Server)
		return (bool)m_Server->IsConnected();
	return false;
}

bool DemoPlayer::IsActive()
{
	return (m_PlayerState != 0);
}

bool DemoPlayer::IsEditMode()
{
	return m_EditorMode;
}

bool DemoPlayer::IsMasterMode()
{
	return m_MasterMode;
}

void DemoPlayer::RemoveFrames(double startTime, double endTime)
{
	(void)startTime;
	(void)endTime;
}

bool DemoPlayer::ExecuteDirectorCmd(DirectorCmd *cmd)
{
	if (!cmd)
		return false;

	int type = cmd->GetType();
	if (type == 4)
	{
		float position[3], angles[3], timescale;
		int entity;
		cmd->GetCameraData(position, angles, timescale, entity);
		cmd->SetCameraData(position, angles, timescale, 0);
		cmd->WriteToStream(&m_DemoStream);
		cmd->SetCameraData(position, angles, timescale, entity);
		return true;
	}
	else if (type == 5)
	{
		float timescale = 1.0f;
		cmd->GetTimeScaleData(timescale);
		SetTimeScale(timescale);
		return true;
	}
	else
	{
		cmd->WriteToStream(&m_DemoStream);
		return true;
	}
}

double DemoPlayer::GetWorldTime()
{
	return m_WorldTime;
}

double DemoPlayer::GetStartTime()
{
	if (!m_World)
		return 0.0;
	float *st = (float *)m_World->GetFirstFrame();
	return st ? (double)*st : 0.0;
}

double DemoPlayer::GetEndTime()
{
	if (!m_World)
		return 0.0;
	float *et = (float *)m_World->GetLastFrame();
	return et ? (double)*et : 0.0;
}

float DemoPlayer::GetTimeScale()
{
	return m_TimeScale;
}

IWorld *DemoPlayer::GetWorld()
{
	return m_World;
}

char *DemoPlayer::GetFileName()
{
	return m_DemoFileName;
}

bool DemoPlayer::SaveGame(char *name)
{
	if (IsLoading() || !m_World)
		return false;

	SetPaused(true);
	m_IsSaving = true;

	const char *fileToSave = name ? name : m_DemoFileName;
	bool res = m_World->SaveAsDemo(fileToSave, (IDirector *)this);
	m_IsSaving = false;
	return res;
}

bool DemoPlayer::LoadGame(char *name)
{
	if (!m_Server || !m_World)
		return false;

	if (m_Server->LoadDemo(m_World, name, m_ForceHLTV, false))
	{
		strncpy(m_DemoFileName, name, sizeof(m_DemoFileName) - 1);
		m_DemoFileName[sizeof(m_DemoFileName) - 1] = '\0';
		m_IsSaving = false;
		m_World->SetBufferSize(-1.0f);
		m_Outgoing_sequence = 0;
		m_LastClockUpdateTime = 0.0;
		m_LastFrameTime = 0.0;
		m_PlayerState = 1;
		m_MasterMode = true;
		return true;
	}
	return false;
}

void DemoPlayer::Stop()
{
	if (m_Server)
		m_Server->Disconnect();
	if (m_World)
		m_World->Reset();
	m_PlayerState = 0;
}

void DemoPlayer::ForceHLTV(bool force)
{
	m_ForceHLTV = force;
}

bool DemoPlayer::GetDemoViewInfo(ref_params_s *params, float *view, int *viewmodel)
{
	if (!m_World || !params)
		return false;

	void *curFrame = m_World->GetFrameBySeqNr(m_LastFrameSeqNr);
	void *prevFrame = m_World->GetFrameBySeqNr(m_LastFrameSeqNr - 1);

	if (!curFrame)
		return false;

	uintptr_t curDemoInfo = *(uintptr_t *)((char *)curFrame + 84);
	if (!curDemoInfo)
		return false;

	int oldViewport[4];
	memcpy(oldViewport, params->viewport, sizeof(oldViewport));
	movevars_s *oldMv = params->movevars;
	usercmd_s *oldCmd = params->cmd;

	memcpy(params->vieworg, (float *)(curDemoInfo + 4), 58 * sizeof(float));

	memcpy(params->viewport, oldViewport, sizeof(oldViewport));
	params->movevars = oldMv;
	params->cmd = oldCmd;

	if (view)
	{
		view[0] = *(float *)(curDemoInfo + 420);
		view[1] = *(float *)(curDemoInfo + 424);
		view[2] = *(float *)(curDemoInfo + 428);
	}
	if (viewmodel)
	{
		*viewmodel = *(int *)(curDemoInfo + 432);
	}

	if (prevFrame)
	{
		float *prevDemoInfo = *(float **)((char *)prevFrame + 84);
		if (prevDemoInfo)
		{
			float prevTime = *(float *)prevFrame;
			float curTime = *(float *)curFrame;
			if (prevTime < curTime)
			{
				float frac = (float)((m_WorldTime - prevTime) / (curTime - prevTime));

				params->vieworg[0] = prevDemoInfo[1] + (*(float *)(curDemoInfo + 4) - prevDemoInfo[1]) * frac;
				params->vieworg[1] = prevDemoInfo[2] + (*(float *)(curDemoInfo + 8) - prevDemoInfo[2]) * frac;
				params->vieworg[2] = prevDemoInfo[3] + (*(float *)(curDemoInfo + 12) - prevDemoInfo[3]) * frac;

				float dPitch = *(float *)(curDemoInfo + 16) - prevDemoInfo[4];
				if (dPitch > 180.0f)
					dPitch -= 360.0f;
				else if (dPitch < -180.0f)
					dPitch += 360.0f;
				params->viewangles[0] = prevDemoInfo[4] + dPitch * frac;

				float dYaw = *(float *)(curDemoInfo + 20) - prevDemoInfo[5];
				if (dYaw > 180.0f)
					dYaw -= 360.0f;
				else if (dYaw < -180.0f)
					dYaw += 360.0f;
				params->viewangles[1] = prevDemoInfo[5] + dYaw * frac;

				float dRoll = *(float *)(curDemoInfo + 24) - prevDemoInfo[6];
				if (dRoll > 180.0f)
					dRoll -= 360.0f;
				else if (dRoll < -180.0f)
					dRoll += 360.0f;
				params->viewangles[2] = prevDemoInfo[6] + dRoll * frac;

				NormalizeAngles(params->viewangles);

				params->simvel[0] = prevDemoInfo[23] + (*(float *)(curDemoInfo + 92) - prevDemoInfo[23]) * frac;
				params->simvel[1] = prevDemoInfo[24] + (*(float *)(curDemoInfo + 96) - prevDemoInfo[24]) * frac;
				params->simvel[2] = prevDemoInfo[25] + (*(float *)(curDemoInfo + 100) - prevDemoInfo[25]) * frac;

				params->simorg[0] = prevDemoInfo[26] + (*(float *)(curDemoInfo + 104) - prevDemoInfo[26]) * frac;
				params->simorg[1] = prevDemoInfo[27] + (*(float *)(curDemoInfo + 108) - prevDemoInfo[27]) * frac;
				params->simorg[2] = prevDemoInfo[28] + (*(float *)(curDemoInfo + 112) - prevDemoInfo[28]) * frac;

				params->viewheight[0] = prevDemoInfo[29] + (*(float *)(curDemoInfo + 116) - prevDemoInfo[29]) * frac;
				params->viewheight[1] = prevDemoInfo[30] + (*(float *)(curDemoInfo + 120) - prevDemoInfo[30]) * frac;
				params->viewheight[2] = prevDemoInfo[31] + (*(float *)(curDemoInfo + 124) - prevDemoInfo[31]) * frac;

				if (view)
				{
					view[0] = prevDemoInfo[105] + (*(float *)(curDemoInfo + 420) - prevDemoInfo[105]) * frac;
					view[1] = prevDemoInfo[106] + (*(float *)(curDemoInfo + 424) - prevDemoInfo[106]) * frac;
					view[2] = prevDemoInfo[107] + (*(float *)(curDemoInfo + 428) - prevDemoInfo[107]) * frac;
				}
			}
		}
	}
	return true;
}

int DemoPlayer::ReadDemoMessage(unsigned char *buffer, int size)
{
	int prevSeqNr = m_LastFrameSeqNr;
	switch (m_PlayerState)
	{
	case 0:
	case 1:
		return 0;

	case 2:
		if (m_World)
			m_World->WriteNewData(&m_DemoStream);
		m_PlayerState = 3;
		break;

	case 3:
		m_LastFrameSeqNr = 0;
		m_DeltaFrameSeqNr = 0;
		WriteSpawn(&m_DemoStream);
		if (m_Engine)
			m_Engine->SetCvar("spec_pip", "0");
		m_StartTime = m_PlayerTime;
		m_WorldTime = 0.0;
		SetTimeScale(1.0f);
		SetPaused(false);
		m_PlayerState = 4;
		break;

	case 4:
		WriteDatagram(&m_DemoStream);
		if (m_World && prevSeqNr < m_LastFrameSeqNr)
		{
			for (int seq = prevSeqNr + 1; seq <= m_LastFrameSeqNr; seq++)
			{
				void *frame = m_World->GetFrameBySeqNr(seq);
				if (frame)
				{
					void *cmdBuf = *(void **)((char *)frame + 76);
					int cmdSize = *(int *)((char *)frame + 80);
					if (cmdBuf && cmdSize > 0)
					{
						BitBuffer stream(cmdBuf, cmdSize);
						ExecuteDemoFileCommands(&stream);
					}
				}
			}
		}
		break;

	default:
		break;
	}

	int streamSize = m_DemoStream.CurrentSize();
	if (streamSize <= 0)
	{
		return 0;
	}

	if (streamSize <= size)
	{
		memcpy(buffer, m_DemoStream.data, streamSize);
		m_DemoStream.FastClear();
		return streamSize;
	}

	if (m_System)
	{
		m_System->Printf("ERROR! DemoPlayer::ReadDemoMessage: data overflow (%i bytes).\n", streamSize);
	}
	m_DemoStream.Clear();
	return 0;
}

void DemoPlayer::ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged,
    int *incoming_reliable_acknowledged, int *incoming_reliable_sequence,
    int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence)
{
	if (incoming_sequence)
		*incoming_sequence = m_Outgoing_sequence;
	if (incoming_acknowledged)
		*incoming_acknowledged = m_Outgoing_sequence;
	if (incoming_reliable_acknowledged)
		*incoming_reliable_acknowledged = 0;
	if (incoming_reliable_sequence)
		*incoming_reliable_sequence = 0;
	if (outgoing_sequence)
		*outgoing_sequence = m_Outgoing_sequence;
	if (reliable_sequence)
		*reliable_sequence = 0;
	if (last_reliable_sequence)
		*last_reliable_sequence = 0;
}

void DemoPlayer::WriteSpawn(BitBuffer *stream)
{
	if (!m_World)
		return;

	m_World->WriteSigonData(stream);
	stream->WriteByte(7);
	stream->WriteFloat(1.0f);

	int clientCount = m_World->GetMaxClients();
	for (int i = 0; i < clientCount; i++)
	{
		m_World->WriteClientUpdate(stream, i);
	}

	m_World->WriteLightStyles(stream);
	stream->WriteByte(25);
	stream->WriteByte(1);
}

void DemoPlayer::WriteDatagram(BitBuffer *stream)
{
	if (!m_World)
		return;

	void *timeInfo = m_World->GetFrameByTime(m_WorldTime);
	if (!timeInfo)
		return;

	unsigned int targetSeq = *(unsigned int *)((char *)timeInfo + 4);
	if (m_LastFrameSeqNr && m_LastFrameSeqNr <= (int)targetSeq)
	{
		if (m_LastFrameSeqNr >= (int)targetSeq)
			return;
	}
	else
	{
		m_LastFrameSeqNr = targetSeq - 1;
		if ((int)targetSeq - 1 >= (int)*(unsigned int *)((char *)timeInfo + 4))
			return;
	}

	stream->WriteByte(7);
	float c = (float)(m_PlayerTime - (m_WorldTime - (double)*(float *)timeInfo));
	stream->WriteFloat(c);

	m_World->WriteFrame(timeInfo, m_LastFrameSeqNr, stream, stream, m_DeltaFrameSeqNr, m_Outgoing_sequence, true);

	if (m_MasterMode)
	{
		WriteCommands(stream, (float)m_LastFrameTime, (float)m_WorldTime);
	}

	m_LastFrameTime = m_WorldTime;

	if (stream->sizeError)
	{
		if (m_System)
			m_System->Printf("Demo data stream overflow.\n");
		stream->Clear();
		m_DeltaFrameSeqNr = 0;
		m_LastFrameSeqNr = 0;
	}
	else
	{
		m_Outgoing_sequence++;
		m_DeltaFrameSeqNr = targetSeq;
		m_LastFrameSeqNr = targetSeq;
	}
}

void DemoPlayer::WriteCameraPath(DirectorCmd *cmd, BitBuffer *stream)
{
	ObjectList path;
	path.Init();

	float startTime = cmd->GetTime();
	DirectorCmd *found = (DirectorCmd *)m_Commands.FindExactKey(cmd->m_Time);
	if (found)
	{
		bool first = true;
		do
		{
			if (found->GetType() != 14)
				break;

			float v[3], fov;
			int flags;
			found->GetCamPathData(v, v, fov, flags);
			if (flags & 1)
			{
				if (!first)
					break;
				first = false;
			}

			path.AddTail(found);
			found = (DirectorCmd *)m_Commands.GetNext();
		} while (found);
	}

	DirectorCmd *firstNode = (DirectorCmd *)path.GetFirst();
	if (!firstNode)
	{
		if (m_System)
			m_System->Printf("Warning! No waypoints in camera path!\n");
		return;
	}

	int elemCount = path.CountElements();
	int totalSize = elemCount * (firstNode->m_Size + 2) + 2;
	if (totalSize > 250)
	{
		if (m_System)
			m_System->Printf("Warning! Too many waypoints in a camera path!\n");
		return;
	}

	stream->WriteByte(51); // svc_director
	stream->WriteByte(totalSize);
	stream->WriteByte(15); // DRC_CMD_WAYPOINTS
	stream->WriteByte(elemCount);

	for (DirectorCmd *node = firstNode; node; node = (DirectorCmd *)path.GetNext())
	{
		float relTime = (node->GetTime() - startTime) * 100.0f;
		stream->WriteShort((int)relTime);
		stream->WriteBuf(node->m_Data.data, node->m_Size);
	}

	path.Clear(false);
}

void DemoPlayer::ExecuteDemoFileCommands(BitBuffer *stream)
{
	if (!stream || !m_Engine)
		return;

	alignas(16) char szCmdName[32768];

	while (true)
	{
		int byteCmd = stream->ReadByte();
		if (byteCmd == -1)
			break;

		switch (byteCmd)
		{
		case 3:
			stream->ReadBuf(64, szCmdName);
			szCmdName[63] = '\0';
			if (m_Engine->ValidStuffText(szCmdName))
			{
				m_Engine->Cbuf_AddFilteredText(szCmdName);
				m_Engine->Cbuf_AddFilteredText("\n");
			}
			else if (m_System)
			{
				m_System->Printf("Demo tried to send invalid command:\"%s\"\n", szCmdName);
			}
			break;

		case 4:
			stream->ReadBuf(32, szCmdName);
			m_Engine->DemoUpdateClientData(szCmdName);
			break;

		case 6:
		{
			int anim = LittleLong(stream->ReadLong());
			int target = LittleLong(stream->ReadLong());
			float time = LittleFloat(stream->ReadFloat());
			stream->ReadBuf(72, szCmdName);
			m_Engine->CL_QueueEvent(anim, target, time, szCmdName);
			break;
		}

		case 7:
		{
			int sound = LittleLong(stream->ReadLong());
			int body = LittleLong(stream->ReadLong());
			m_Engine->HudWeaponAnim(sound, body);
			break;
		}

		case 8:
		{
			unsigned int anim = stream->ReadLong();
			unsigned int len = stream->ReadLong();
			if (len > 256)
				len = 256;
			stream->ReadBuf((int)len, szCmdName);
			szCmdName[len] = '\0';
			float time1 = LittleFloat(stream->ReadFloat());
			float time2 = LittleFloat(stream->ReadFloat());
			int flags = LittleLong(stream->ReadLong());
			int pitch = LittleLong(stream->ReadLong());
			m_Engine->CL_DemoPlaySound((int)anim, szCmdName, time1, time2, pitch, flags);
			break;
		}

		case 9:
		{
			memset(szCmdName, 0, sizeof(szCmdName));
			unsigned int len = stream->ReadLong();
			if (len > sizeof(szCmdName))
				len = sizeof(szCmdName);
			stream->ReadBuf((int)len, szCmdName);
			m_Engine->ClientDLL_ReadDemoBuffer((int)len, (unsigned char *)szCmdName);
			break;
		}

		default:
			if (m_System)
			{
				m_System->Printf("WARNING! DemoPlayer::ExecuteDemoFileCommands: unexpected demo file command %i\n", byteCmd);
			}
			return;
		}
	}
}

void DemoPlayer::CMD_Jump(char *cmdLine)
{
	if (!IsActive())
	{
		if (m_System)
			m_System->Printf("Not viewing a demo.\n");
		return;
	}

	TokenLine params(cmdLine);
	if (params.CountToken() == 2)
	{
		double seconds = strtod(params.GetToken(1), nullptr);
		SetWorldTime(seconds, true);
		SetPaused(true);
	}
	else if (m_System)
	{
		m_System->Printf("dem_jump <seconds>\n");
	}
}

void DemoPlayer::CMD_ForceHLTV(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() == 2)
	{
		int val = (int)strtol(params.GetToken(1), nullptr, 10);
		ForceHLTV(val == 1);
	}
	else if (m_System)
	{
		m_System->Printf("dem_forcehltv <0|1>\n");
	}
}

void DemoPlayer::CMD_Pause(char *cmdLine)
{
	if (!IsActive())
	{
		if (m_System)
			m_System->Printf("Not viewing a demo.\n");
		return;
	}

	TokenLine params(cmdLine);
	if (params.CountToken() == 2)
	{
		int val = (int)strtol(params.GetToken(1), nullptr, 10);
		SetPaused(val == 1);
	}
	else if (params.CountToken() == 1)
	{
		SetPaused(!m_IsPaused);
	}
	else if (m_System)
	{
		m_System->Printf("dem_pause <0|1>\n");
	}
}

void DemoPlayer::CMD_Speed(char *cmdLine)
{
	if (!IsActive())
	{
		if (m_System)
			m_System->Printf("Not viewing a demo.\n");
		return;
	}

	TokenLine params(cmdLine);
	if (params.CountToken() == 2)
	{
		float speed = (float)strtod(params.GetToken(1), nullptr);
		SetTimeScale(speed);
	}
	else if (params.CountToken() == 1)
	{
		if (m_System)
			m_System->Printf("dem_speed: current speed is %.2fx\n", (double)m_TimeScale);
	}
	else if (m_System)
	{
		m_System->Printf("dem_speed <replayspeed>\n");
	}
}

void DemoPlayer::CMD_Start(char *cmdLine)
{
	(void)cmdLine;
	if (!IsActive())
	{
		if (m_System)
			m_System->Printf("Not viewing a demo.\n");
		return;
	}

	SetWorldTime(GetStartTime(), false);
}

void DemoPlayer::CMD_Save(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() == 2)
	{
		char *filename = params.GetToken(1);
		if (!SaveGame(filename))
		{
			if (m_System)
				m_System->Printf("Warning! Could not save game as demo file.\n");
		}
	}
	else if (m_System)
	{
		m_System->Printf("dem_save <filename>\n");
	}
}

// Global singleton instance and factory
static DemoPlayer g_DemoPlayerSingleton;

DemoPlayer *CreateDemoPlayer()
{
	return &g_DemoPlayerSingleton;
}

static InterfaceReg __g_CreateDemoPlayer_reg((InstantiateInterfaceFn)CreateDemoPlayer, DEMOPLAYER_INTERFACE_VERSION);
