#ifndef IMGUI_SCOREBOARD_H
#define IMGUI_SCOREBOARD_H

#include "imgui_window.h"
#include "ui_utils.h"
#include "hud.h"
#include "cl_util.h"

#define MAX_SCOREBOARD_TEAMS 5
#define NUM_ROWS (MAX_PLAYERS + (MAX_SCOREBOARD_TEAMS * 2))
#define TEAM_NO 0
#define TEAM_YES 1
#define TEAM_SPECTATORS 2
#define TEAM_BLANK 3
struct TeamData
{
	int iFrags = 0;
	int iDeaths = 0;
	int iPlayerCount = 0;
};

struct PlayerData
{
	bool bIsConnected = false;
	int nItemID = -1;
	int nTeamID = 0;
};


class CImGuiScoreboard : public IImGuiWindow
{
public:
    static bool m_ShowWindow;

    void Initialize();
    void VidInitialize();
    void Terminate();
    void Think();

    void DrawScoreboard();
    void Draw();
    bool Active();
    bool CursorRequired();
    bool HandleKey(bool keyDown, int keyNumber, const char *bindName);

    void SortTeams();
    void SortPlayers(int iTeam, char *team);
    void RebuildTeams();
    void DeathMsg( int killer, int victim );

    int m_iRows;
    int m_iNumTeams;

    int m_iSortedRows[NUM_ROWS];
    int m_iIsATeam[NUM_ROWS];
    bool m_bHasBeenSorted[MAX_PLAYERS];

    int m_iPlayerNum;
    int m_iLastKilledBy;
    int m_fLastKillTime;
	std::array<TeamData, MAX_TEAMS + 2> m_TeamData;
	std::array<bool, MAX_TEAMS + 2> m_IsTeamSectionCreated;
	std::array<PlayerData, MAX_PLAYERS + 1> m_PlayerData;

};

extern CImGuiScoreboard m_iScoreboard;

#endif // IMGUI_SCOREBOARD_H
