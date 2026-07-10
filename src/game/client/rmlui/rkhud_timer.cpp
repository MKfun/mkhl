#include "rkhud_timer.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>

RkHudRoundTimer RkHudRoundTimer::m_Instance = RkHudRoundTimer("hud_roundtimer");
RkHudRoundTimer::RoundTimerData RkHudRoundTimer::roundTimerData = {
	0,
	0
};

documentReloadFuncs roundTimerDocReloadFuncs;

void UnloadRkRoundTimer()
{
	RkHudRoundTimer &pTimer = RkHudRoundTimer::m_Instance;
	if (!pTimer.m_pInstance)
	{
		Warning("Couldn't grab RkHudRoundTimer element to unload!\n");
		return;
	}

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (hudCtx)
	{
		hudCtx->RemoveDataModel("roundtimer_model");
		pTimer.m_dataModel = nullptr;
	}
	else
	{
		Warning("Couldn't access hudCtx to unload timer datamodel\n");
	}
	// pInfoBar.m_dataModel = nullptr;

	// if (pInfoBar.m_pInstance)
	{
		pTimer.m_pInstance->Close();
		pTimer.m_pInstance = nullptr;
	}
}
void LoadRkRoundTimer()
{
	roundTimerDocReloadFuncs.LoadDocument = &LoadRkRoundTimer;
	roundTimerDocReloadFuncs.UnloadDocument = &UnloadRkRoundTimer;

	RkHudRoundTimer &pTimer = RkHudRoundTimer::m_Instance;

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (!hudCtx)
	{
		Error("Couldn't access hudctx!\n");
		return;
	}

	if (pTimer.m_pInstance || pTimer.m_dataModel)
	{
		Warning("RkHudRoundTimer already loaded, call unload first!\n");
		return;
	}

	Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("roundtimer_model");
	if (!constructor)
	{
		Error("Couldn't create datamodel for timer!\n");
		return;
	}

	constructor.Bind("minutes_left", &RkHudRoundTimer::roundTimerData.MinutesLeft);
	constructor.Bind("seconds_left", &RkHudRoundTimer::roundTimerData.SecondsLeft);

	pTimer.m_dataModel = constructor.GetModelHandle();

	pTimer.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_roundtimer.rml", &roundTimerDocReloadFuncs);

	if (!pTimer.m_pInstance)
	{
		Error("Couldn't create hud_roundtimer document!\n");
		return;
	}
	pTimer.SetActive(1);
	pTimer.m_pInstance->Show();
	pTimer.m_pInstance->PullToFront();
}

RkHudRoundTimer::RkHudRoundTimer(const char *value)
    : m_bVisible(false)
    , m_pInstance(nullptr)
{
}

RkHudRoundTimer::~RkHudRoundTimer() noexcept
{
	UnloadRkRoundTimer();
}

void RkHudRoundTimer::LevelInit()
{
	LoadRkRoundTimer();
}

void RkHudRoundTimer::LevelShutdown()
{
	UnloadRkRoundTimer();
}
int RkHudRoundTimer::m_iRemainingTime = 0;
void RkHudRoundTimer::ShowPanel(bool bShow, bool force)
{
	if (!m_pInstance)
		return;

	if (bShow)
	{
		if (!m_bVisible)
		{
			m_pInstance->Show();
		}

		roundTimerData.SecondsLeft = m_iRemainingTime % 60;
		roundTimerData.MinutesLeft = m_iRemainingTime / 60;

		m_dataModel.DirtyVariable("minutes_left");
		m_dataModel.DirtyVariable("seconds_left");

		// m_dataModel.Update();
	}
	else
	{
		if (m_bVisible)
		{
			m_pInstance->Hide();
		}
	}

	m_bVisible = bShow;
}

void RkHudRoundTimer::SetActive(bool bActive)
{
	ShowPanel(bActive, false);
}

bool RkHudRoundTimer::ShouldDraw()
{

	return 1;
}