#include "rkhud_speedometer.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>

RkHudSpeedometer RkHudSpeedometer::m_Instance = RkHudSpeedometer("hud_speedometer");

int RkHudSpeedometer::m_iJumpSpeed = 0;
int RkHudSpeedometer::m_iSpeed = 0;
documentReloadFuncs SpeedometerDocReloadFuncs;

void UnloadRkSpeedometer()
{
	RkHudSpeedometer &pSpdMeter = RkHudSpeedometer::m_Instance;
	if (!pSpdMeter.m_pInstance)
	{
		Warning("Couldn't grab RkHudSpeedometer element to unload!\n");
		return;
	}

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (hudCtx)
	{
		hudCtx->RemoveDataModel("speedometer_model");
		pSpdMeter.m_dataModel = nullptr;
	}
	else
	{
		Warning("Couldn't access hudCtx to unload speedometer datamodel\n");
	}
	// pInfoBar.m_dataModel = nullptr;

	// if (pInfoBar.m_pInstance)
	{
		pSpdMeter.m_pInstance->Close();
		pSpdMeter.m_pInstance = nullptr;
	}
}
void LoadRkSpeedometer()
{
	SpeedometerDocReloadFuncs.LoadDocument = &LoadRkSpeedometer;
	SpeedometerDocReloadFuncs.UnloadDocument = &UnloadRkSpeedometer;

	RkHudSpeedometer &pSpdMeter = RkHudSpeedometer::m_Instance;

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (!hudCtx)
	{
		Error("Couldn't access hudctx!\n");
		return;
	}

	if (pSpdMeter.m_pInstance || pSpdMeter.m_dataModel)
	{
		Warning("RkHudSpeedometer already loaded, call unload first!\n");
		return;
	}

	Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("speedometer_model");
	if (!constructor)
	{
		Error("Couldn't create datamodel for speedometer!\n");
		return;
	}

	constructor.Bind("speed", &RkHudSpeedometer::m_iSpeed);
	constructor.Bind("jumpspeed", &RkHudSpeedometer::m_iJumpSpeed);

	pSpdMeter.m_dataModel = constructor.GetModelHandle();

	pSpdMeter.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_speedometer.rml", &SpeedometerDocReloadFuncs);

	if (!pSpdMeter.m_pInstance)
	{
		Error("Couldn't create hud_speedometer document!\n");
		return;
	}
	pSpdMeter.SetActive(1);
	pSpdMeter.m_pInstance->Show();
	pSpdMeter.m_pInstance->PullToFront();
}

RkHudSpeedometer::RkHudSpeedometer(const char *value)
    : m_bVisible(false)
    , m_pInstance(nullptr)
{
}

RkHudSpeedometer::~RkHudSpeedometer() noexcept
{
	UnloadRkSpeedometer();
}

void RkHudSpeedometer::LevelInit()
{
	LoadRkSpeedometer();
}

void RkHudSpeedometer::LevelShutdown()
{
	UnloadRkSpeedometer();
}
void RkHudSpeedometer::ShowPanel(bool bShow, bool force)
{
	if (!m_pInstance)
		return;

	if (bShow)
	{
		if (!m_bVisible)
		{
			m_pInstance->Show();
		}

		m_dataModel.DirtyVariable("speed");
		m_dataModel.DirtyVariable("jumpspeed");

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

void RkHudSpeedometer::SetActive(bool bActive)
{
	ShowPanel(bActive, false);
}

bool RkHudSpeedometer::ShouldDraw()
{

	return 1;
}