#include "director_cmd.h"
#include <cstdio>
#include <cstring>

const char *DRC_CMD_NAME[16] = {
	"NONE",
	"START",
	"EVENT",
	"MODE",
	"CAMERA",
	"TIMESCALE",
	"MESSAGE",
	"SOUND",
	"STATUS",
	"BANNER",
	"STUFFTEXT",
	"CHASE",
	"INEYE",
	"MAP",
	"CAMPATH",
	"WAYPOINTS"
};

DirectorCmd::DirectorCmd()
{
	m_Time = 0.0f;
	m_Type = 0;
	m_Size = 0;
	m_Index = 0;
	m_Data.Free();
}

DirectorCmd::~DirectorCmd()
{
	m_Type = 0;
	m_Time = 0.0f;
	m_Data.Free();
}

void DirectorCmd::Copy(DirectorCmd *cmd)
{
	if (!cmd)
		return;

	m_Type = 0;
	m_Time = 0.0f;
	m_Data.Free();

	m_Time = cmd->m_Time;
	m_Type = cmd->m_Type;
	m_Size = cmd->m_Size;
	m_Index = cmd->m_Index;

	m_Data.Resize(m_Size);
	m_Data.WriteBuf(cmd->m_Data.data, m_Size);
}

void DirectorCmd::Clear()
{
	m_Type = 0;
	m_Time = 0.0f;
	m_Data.Free();
}

void DirectorCmd::Resize(int size)
{
	m_Data.Resize(size);
	m_Size = size;
}

float DirectorCmd::GetTime()
{
	return m_Time;
}

void DirectorCmd::SetTime(float time)
{
	m_Time = time;
}

int DirectorCmd::GetType()
{
	return m_Type;
}

char *DirectorCmd::GetName()
{
	if (m_Type >= 0 && m_Type < 16)
		return (char *)DRC_CMD_NAME[m_Type];
	return (char *)"UNKNOWN";
}

bool DirectorCmd::ReadFromStream(BitBuffer *stream)
{
	if (!stream)
		return false;

	m_Type = 0;
	m_Time = 0.0f;
	m_Data.Free();

	int type = stream->ReadByte();
	m_Type = type;

	switch (type)
	{
	case 1: // DRC_CMD_START
		m_Data.Resize(0);
		m_Size = 0;
		return true;

	case 2: // DRC_CMD_EVENT
		m_Data.Resize(8);
		m_Size = 8;
		m_Data.WriteBuf(stream, 8);
		return true;

	case 3:  // DRC_CMD_MODE
	case 12: // DRC_CMD_INEYE
		m_Data.Resize(1);
		m_Size = 1;
		m_Data.WriteBuf(stream, 1);
		return true;

	case 4: // DRC_CMD_CAMERA
		m_Data.Resize(15);
		m_Size = 15;
		m_Data.WriteBuf(stream, 15);
		return true;

	case 5: // DRC_CMD_TIMESCALE
		m_Data.Resize(4);
		m_Size = 4;
		m_Data.WriteBuf(stream, 4);
		return true;

	case 6: // DRC_CMD_MESSAGE
	{
		unsigned char *buf = stream->currentByte;
		stream->SkipBytes(29);
		char *str = stream->ReadString();
		int strLen = (int)strlen(str);
		m_Data.Resize(strLen + 30);
		m_Size = strLen + 30;
		m_Data.WriteBuf(buf, 29);
		m_Data.WriteBuf(str, strLen + 1);
		return true;
	}

	case 7: // DRC_CMD_SOUND
	{
		char *sndName = stream->ReadString();
		int sndLen = (int)strlen(sndName);
		m_Data.Resize(sndLen + 5);
		m_Size = sndLen + 5;
		m_Data.WriteBuf(sndName, sndLen + 1);
		float vol = stream->ReadFloat();
		m_Data.WriteFloat(vol);
		return true;
	}

	case 8: // DRC_CMD_STATUS
		m_Data.Resize(10);
		m_Size = 10;
		m_Data.WriteBuf(stream, 10);
		return true;

	case 9:  // DRC_CMD_BANNER
	case 10: // DRC_CMD_STUFFTEXT
	{
		char *txt = stream->ReadString();
		int txtLen = (int)strlen(txt) + 1;
		m_Data.Resize(txtLen);
		m_Size = txtLen;
		m_Data.WriteBuf(txt, txtLen);
		return true;
	}

	case 11: // DRC_CMD_CHASE
		m_Data.Resize(7);
		m_Size = 7;
		m_Data.WriteBuf(stream, 7);
		return true;

	case 13: // DRC_CMD_MAP
		m_Data.Resize(10);
		m_Size = 10;
		m_Data.WriteBuf(stream, 10);
		return true;

	case 14: // DRC_CMD_CAMPATH
		m_Data.Resize(14);
		m_Size = 14;
		m_Data.WriteBuf(stream, 14);
		return true;

	case 15: // DRC_CMD_WAYPOINTS
	{
		m_Data.Resize(1);
		m_Size = 1;
		int count = stream->ReadByte();
		m_Data.WriteByte(count);
		stream->SkipBytes(14 * count);
		return true;
	}

	default:
		return false;
	}
}

void DirectorCmd::WriteToStream(BitBuffer *stream)
{
	if (!stream)
		return;

	if (m_Type > 0 && m_Type <= 15 && m_Size <= 254)
	{
		stream->WriteByte(51); // svc_director
		stream->WriteByte(m_Size + 1);
		stream->WriteByte(m_Type);
		stream->WriteBuf(m_Data.data, m_Size);
	}
}

char *DirectorCmd::ToString()
{
	static char s[1024];
	memset(s, 0, sizeof(s));

	if (m_Type <= 0 || m_Type > 15)
		return nullptr;

	const char *name = DRC_CMD_NAME[m_Type];
	float v1[3], v2[3], f1, f2, f3, f4;
	int i1, i2;
	char t2[1024];

	switch (m_Type)
	{
	case 1:
		snprintf(s, sizeof(s), "%s", name);
		return s;

	case 2:
		m_Data.Reset();
		i1 = m_Data.ReadWord();
		i2 = m_Data.ReadWord();
		f1 = (float)m_Data.ReadLong();
		snprintf(s, sizeof(s), "%s %i %i %i", name, i1, i2, (int)f1);
		return s;

	case 3:
	case 12:
	case 15:
		m_Data.Reset();
		i1 = m_Data.ReadByte();
		snprintf(s, sizeof(s), "%s %i", name, i1);
		return s;

	case 4:
		GetCameraData(v1, v2, f1, i1);
		snprintf(s, sizeof(s), "%s (%.1f %.1f %.1f) (%.1f %.1f %.1f) %.1f %i",
			name, v1[0], v1[1], v1[2], v2[0], v2[1], v2[2], f1, i1);
		return s;

	case 5:
		m_Data.Reset();
		f1 = m_Data.ReadFloat();
		snprintf(s, sizeof(s), "%s %.2f", name, (double)f1);
		return s;

	case 6:
		GetMessageData(i1, i2, v1, f1, f2, f3, f4, t2);
		snprintf(s, sizeof(s), "%s \"%s\" %i %x (%.2f %.2f) %.1f, %.1f %.1f %.1f",
			name, t2, i1, i2, v1[0], v1[1], f1, f2, f3, f4);
		return s;

	case 7:
		m_Data.Reset();
		strncpy(t2, m_Data.ReadString(), sizeof(t2) - 1);
		t2[sizeof(t2) - 1] = '\0';
		f1 = m_Data.ReadFloat();
		snprintf(s, sizeof(s), "%s \"%s\" %.2f", name, t2, (double)f1);
		return s;

	case 8:
		m_Data.Reset();
		i1 = m_Data.ReadLong();
		i2 = m_Data.ReadLong();
		f1 = (float)m_Data.ReadWord();
		snprintf(s, sizeof(s), "%s %i %i %i", name, i1, i2, (int)f1);
		return s;

	case 9:
	case 10:
		m_Data.Reset();
		strncpy(t2, m_Data.ReadString(), sizeof(t2) - 1);
		t2[sizeof(t2) - 1] = '\0';
		snprintf(s, sizeof(s), "%s \"%s\"", name, t2);
		return s;

	case 11:
		m_Data.Reset();
		i1 = m_Data.ReadByte();
		i2 = m_Data.ReadByte();
		f1 = m_Data.ReadFloat();
		f2 = (float)m_Data.ReadByte();
		snprintf(s, sizeof(s), "%s %i %i %.1f %i", name, i1, i2, f1, (int)f2);
		return s;

	case 13:
		m_Data.Reset();
		i1 = m_Data.ReadByte();
		f1 = m_Data.ReadFloat();
		f2 = m_Data.ReadFloat();
		snprintf(s, sizeof(s), "%s %i %.1f %.1f", name, i1, f1, (double)f2);
		return s;

	case 14:
		GetCamPathData(v1, v2, f1, i1);
		snprintf(s, sizeof(s), "%s (%.1f %.1f %.1f) (%.1f %.1f %.1f) %.1f %i",
			name, v1[0], v1[1], v1[2], v2[0], v2[1], v2[2], f1, i1);
		return s;

	default:
		return s;
	}
}

void DirectorCmd::FromString(char *string)
{
	(void)string;
}

bool DirectorCmd::GetEventData(int &entity1, int &entity2, int &flags)
{
	if (m_Type == 2)
	{
		m_Data.Reset();
		entity1 = m_Data.ReadWord();
		entity2 = m_Data.ReadWord();
		flags = m_Data.ReadLong();
		return true;
	}
	return false;
}

void DirectorCmd::SetEventData(int entity1, int entity2, int flags)
{
	m_Type = 2;
	m_Data.Resize(8);
	m_Size = 8;
	m_Data.WriteWord(entity1);
	m_Data.WriteWord(entity2);
	m_Data.WriteLong(flags);
}

bool DirectorCmd::GetModeData(int &mode)
{
	if (m_Type == 3)
	{
		m_Data.Reset();
		mode = m_Data.ReadByte();
		return true;
	}
	return false;
}

void DirectorCmd::SetModeData(int mode)
{
	m_Type = 3;
	m_Data.Resize(1);
	m_Size = 1;
	m_Data.WriteByte(mode);
}

bool DirectorCmd::GetCameraData(float *position, float *angles, float &fov, int &entity)
{
	if (m_Type == 4)
	{
		m_Data.Reset();
		position[0] = m_Data.ReadCoord();
		position[1] = m_Data.ReadCoord();
		position[2] = m_Data.ReadCoord();
		angles[0] = m_Data.ReadCoord();
		angles[1] = m_Data.ReadCoord();
		angles[2] = m_Data.ReadCoord();
		fov = (float)m_Data.ReadByte();
		entity = m_Data.ReadWord();
		return true;
	}
	return false;
}

void DirectorCmd::SetCameraData(float *position, float *angles, float fov, int entity)
{
	m_Type = 4;
	m_Data.Resize(15);
	m_Size = 15;
	m_Data.WriteCoord(position[0]);
	m_Data.WriteCoord(position[1]);
	m_Data.WriteCoord(position[2]);
	m_Data.WriteCoord(angles[0]);
	m_Data.WriteCoord(angles[1]);
	m_Data.WriteCoord(angles[2]);
	m_Data.WriteByte((int)fov);
	m_Data.WriteWord(entity);
}

bool DirectorCmd::GetTimeScaleData(float &factor)
{
	if (m_Type == 5)
	{
		m_Data.Reset();
		factor = m_Data.ReadFloat();
		return true;
	}
	return false;
}

void DirectorCmd::SetTimeScaleData(float factor)
{
	m_Type = 5;
	m_Data.Resize(4);
	m_Size = 4;
	m_Data.WriteFloat(factor);
}

bool DirectorCmd::GetMessageData(int &effect, int &color, float *position, float &fadein, float &fadeout, float &holdtime, float &fxtime, char *text)
{
	if (m_Type == 6)
	{
		m_Data.Reset();
		effect = m_Data.ReadByte();
		color = m_Data.ReadLong();
		position[0] = m_Data.ReadFloat();
		position[1] = m_Data.ReadFloat();
		fadein = m_Data.ReadFloat();
		fadeout = m_Data.ReadFloat();
		holdtime = m_Data.ReadFloat();
		fxtime = m_Data.ReadFloat();
		strcpy(text, m_Data.ReadString());
		return true;
	}
	return false;
}

void DirectorCmd::SetMessageData(int effect, unsigned int color, float *position, float fadein, float fadeout, float holdtime, float fxtime, char *text)
{
	int len = (int)strlen(text);
	m_Type = 6;
	m_Size = len + 30;
	m_Data.Resize(len + 30);
	m_Data.WriteByte(effect);
	m_Data.WriteLong(color);
	m_Data.WriteFloat(position[0]);
	m_Data.WriteFloat(position[1]);
	m_Data.WriteFloat(fadein);
	m_Data.WriteFloat(fadeout);
	m_Data.WriteFloat(holdtime);
	m_Data.WriteFloat(fxtime);
	m_Data.WriteString(text);
}

bool DirectorCmd::GetSoundData(char *name, float &volume)
{
	if (m_Type == 7)
	{
		m_Data.Reset();
		strcpy(name, m_Data.ReadString());
		volume = m_Data.ReadFloat();
		return true;
	}
	return false;
}

void DirectorCmd::SetSoundData(char *name, float volume)
{
	int len = (int)strlen(name);
	m_Type = 7;
	m_Size = len + 5;
	m_Data.Resize(len + 5);
	m_Data.WriteString(name);
	m_Data.WriteFloat(volume);
}

bool DirectorCmd::GetStatusData(int &slots, int &spectators, int &proxies)
{
	if (m_Type == 8)
	{
		m_Data.Reset();
		slots = m_Data.ReadLong();
		spectators = m_Data.ReadLong();
		proxies = m_Data.ReadWord();
		return true;
	}
	return false;
}

void DirectorCmd::SetStatusData(int slots, int spectators, int proxies)
{
	m_Type = 8;
	m_Data.Resize(10);
	m_Size = 10;
	m_Data.WriteLong(slots);
	m_Data.WriteLong(spectators);
	m_Data.WriteWord(proxies);
}

bool DirectorCmd::GetBannerData(char *filename)
{
	if (m_Type == 9)
	{
		m_Data.Reset();
		strcpy(filename, m_Data.ReadString());
		return true;
	}
	return false;
}

void DirectorCmd::SetBannerData(char *filename)
{
	int len = (int)strlen(filename);
	m_Type = 9;
	m_Size = len + 1;
	m_Data.Resize(len + 1);
	m_Data.WriteString(filename);
}

bool DirectorCmd::GetStuffTextData(char *commands)
{
	if (m_Type == 10)
	{
		m_Data.Reset();
		strcpy(commands, m_Data.ReadString());
		return true;
	}
	return false;
}

void DirectorCmd::SetStuffTextData(char *commands)
{
	int len = (int)strlen(commands);
	m_Type = 10;
	m_Size = len + 1;
	m_Data.Resize(len + 1);
	m_Data.WriteString(commands);
}

bool DirectorCmd::GetChaseData(int &entity1, int &entity2, float &distance, int &flags)
{
	if (m_Type == 11)
	{
		m_Data.Reset();
		entity1 = m_Data.ReadByte();
		entity2 = m_Data.ReadByte();
		distance = m_Data.ReadFloat();
		flags = m_Data.ReadByte();
		return true;
	}
	return false;
}

void DirectorCmd::SetChaseData(int entity1, int entity2, float distance, int flags)
{
	m_Type = 11;
	m_Data.Resize(9);
	m_Size = 9;
	m_Data.WriteWord(entity1);
	m_Data.WriteWord(entity2);
	m_Data.WriteFloat(distance);
	m_Data.WriteByte(flags);
}

bool DirectorCmd::GetInEyeData(int &player)
{
	if (m_Type == 12)
	{
		m_Data.Reset();
		player = m_Data.ReadByte();
		return true;
	}
	return false;
}

void DirectorCmd::SetInEyeData(int player)
{
	m_Type = 12;
	m_Data.Resize(2);
	m_Size = 2;
	m_Data.WriteWord(player);
}

bool DirectorCmd::GetMapData(int &entity, float &angle, float &distance)
{
	if (m_Type == 13)
	{
		m_Data.Reset();
		entity = m_Data.ReadByte();
		angle = m_Data.ReadFloat();
		distance = m_Data.ReadFloat();
		return true;
	}
	return false;
}

void DirectorCmd::SetMapData(int entity, float angle, float distance)
{
	m_Type = 13;
	m_Data.Resize(10);
	m_Size = 10;
	m_Data.WriteWord(entity);
	m_Data.WriteFloat(angle);
	m_Data.WriteFloat(distance);
}

bool DirectorCmd::GetCamPathData(float *position, float *angles, float &fov, int &flags)
{
	if (m_Type == 14)
	{
		m_Data.Reset();
		position[0] = m_Data.ReadCoord();
		position[1] = m_Data.ReadCoord();
		position[2] = m_Data.ReadCoord();
		angles[0] = m_Data.ReadCoord();
		angles[1] = m_Data.ReadCoord();
		angles[2] = m_Data.ReadCoord();
		fov = (float)m_Data.ReadByte();
		flags = m_Data.ReadByte();
		return true;
	}
	return false;
}

void DirectorCmd::SetCamPathData(float *position, float *angles, float fov, int flags)
{
	m_Type = 14;
	m_Data.Resize(14);
	m_Size = 14;
	m_Data.WriteCoord(position[0]);
	m_Data.WriteCoord(position[1]);
	m_Data.WriteCoord(position[2]);
	m_Data.WriteCoord(angles[0]);
	m_Data.WriteCoord(angles[1]);
	m_Data.WriteCoord(angles[2]);
	m_Data.WriteByte((int)fov);
	m_Data.WriteByte(flags);
}

bool DirectorCmd::GetWayPointsData(int &number)
{
	if (m_Type == 15)
	{
		m_Data.Reset();
		number = m_Data.ReadByte();
		return true;
	}
	return false;
}

void DirectorCmd::SetWayPoints(int number)
{
	m_Type = 15;
	m_Data.Resize(1);
	m_Size = 1;
	m_Data.WriteByte(number);
}

void DirectorCmd::SetStartData()
{
	m_Type = 1;
	m_Data.Resize(0);
	m_Size = 0;
}
