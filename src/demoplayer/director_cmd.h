#ifndef DEMOPLAYER_DIRECTOR_CMD_H
#define DEMOPLAYER_DIRECTOR_CMD_H

#include "bitbuffer.h"

extern const char *DRC_CMD_NAME[16];

class DirectorCmd
{
public:
	DirectorCmd();
	virtual ~DirectorCmd();

	void Copy(DirectorCmd *cmd);
	void Clear();
	void Resize(int size);

	float GetTime();
	void SetTime(float time);
	int GetType();
	char *GetName();

	bool ReadFromStream(BitBuffer *stream);
	void WriteToStream(BitBuffer *stream);
	char *ToString();
	void FromString(char *string);

	bool GetEventData(int &entity1, int &entity2, int &flags);
	void SetEventData(int entity1, int entity2, int flags);

	bool GetModeData(int &mode);
	void SetModeData(int mode);

	bool GetCameraData(float *position, float *angles, float &fov, int &entity);
	void SetCameraData(float *position, float *angles, float fov, int entity);

	bool GetTimeScaleData(float &factor);
	void SetTimeScaleData(float factor);

	bool GetMessageData(int &effect, int &color, float *position, float &fadein, float &fadeout, float &holdtime, float &fxtime, char *text);
	void SetMessageData(int effect, unsigned int color, float *position, float fadein, float fadeout, float holdtime, float fxtime, char *text);

	bool GetSoundData(char *name, float &volume);
	void SetSoundData(char *name, float volume);

	bool GetStatusData(int &slots, int &spectators, int &proxies);
	void SetStatusData(int slots, int spectators, int proxies);

	bool GetBannerData(char *filename);
	void SetBannerData(char *filename);

	bool GetStuffTextData(char *commands);
	void SetStuffTextData(char *commands);

	bool GetChaseData(int &entity1, int &entity2, float &distance, int &flags);
	void SetChaseData(int entity1, int entity2, float distance, int flags);

	bool GetInEyeData(int &player);
	void SetInEyeData(int player);

	bool GetMapData(int &entity, float &angle, float &distance);
	void SetMapData(int entity, float angle, float distance);

	bool GetCamPathData(float *position, float *angles, float &fov, int &flags);
	void SetCamPathData(float *position, float *angles, float fov, int flags);

	bool GetWayPointsData(int &number);
	void SetWayPoints(int number);

	void SetStartData();

public:
	float m_Time;
	int m_Type;
	int m_Size;
	BitBuffer m_Data;
	int m_Index;
};

#endif // DEMOPLAYER_DIRECTOR_CMD_H
