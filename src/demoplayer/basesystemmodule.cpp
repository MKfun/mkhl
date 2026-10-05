#include "basesystemmodule.h"
#include <cstring>

BaseSystemModule::BaseSystemModule()
{
	m_System = nullptr;
	m_Serial = 0;
	m_SystemTime = 0.0;
	m_State = 0;
	memset(m_Name, 0, sizeof(m_Name));
}

BaseSystemModule::~BaseSystemModule()
{
	m_Listener.Clear(false);
}

bool BaseSystemModule::Init(IBaseSystem *system, int serial, char *name)
{

	if (!system)
		return false;

	m_State = 1;
	m_System = system;
	m_Serial = serial;
	m_SystemTime = 0.0;

	if (name)
	{
		strncpy(m_Name, name, sizeof(m_Name) - 2);
		m_Name[sizeof(m_Name) - 2] = '\0';
	}

	m_Listener.Init();
	return true;
}

void BaseSystemModule::RunFrame(double time)
{
	m_SystemTime = time;
}

void BaseSystemModule::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	(void)data;
	if (m_System)
	{
		const char *sender = module ? module->GetName() : "unknown";
		m_System->Print("WARNING! Unhandled signal (%i) from module %s.\n", signal, sender);
	}
}

void BaseSystemModule::ExecuteCommand(int cmd, char *str)
{
	(void)cmd;
	(void)str;
	if (m_System)
	{
		m_System->Print("WARNING! Undeclared ExecuteCommand().\n");
	}
}

void BaseSystemModule::RegisterListener(ISystemModule *listener)
{
	if (!listener)
		return;

	for (ISystemModule *mod = (ISystemModule *)m_Listener.GetFirst(); mod; mod = (ISystemModule *)m_Listener.GetNext())
	{
		if (mod->GetSerial() == listener->GetSerial())
		{
			if (m_System)
			{
				m_System->Print("WARNING! BaseSystemModule::RegisterListener: module %s already added.\n", listener->GetName());
			}
			return;
		}
	}

	m_Listener.Add(listener);
}

void BaseSystemModule::RemoveListener(ISystemModule *listener)
{
	if (!listener)
		return;

	for (ISystemModule *mod = (ISystemModule *)m_Listener.GetFirst(); mod; mod = (ISystemModule *)m_Listener.GetNext())
	{
		if (mod->GetSerial() == listener->GetSerial())
		{
			m_Listener.Remove(mod);
			return;
		}
	}
}

IBaseSystem *BaseSystemModule::GetSystem()
{
	return m_System;
}

int BaseSystemModule::GetSerial()
{
	return m_Serial;
}

char *BaseSystemModule::GetStatusLine()
{
	return (char *)"No status available.\n";
}

const char *BaseSystemModule::GetType()
{
	return "GenericModule";
}

char *BaseSystemModule::GetName()
{
	return m_Name;
}

int BaseSystemModule::GetState()
{
	return m_State;
}

float BaseSystemModule::GetVersion()
{
	return 1.0f;
}

void BaseSystemModule::ShutDown()
{
	if (m_State != 4)
	{
		m_Listener.Clear(false);
		m_State = 4;
		if (m_System && !m_System->RemoveModule(this))
		{
			m_System->Print("ERROR! BaseSystemModule::ShutDown: faild to remove module %s.\n", m_Name);
		}
	}
}

char *BaseSystemModule::COM_GetBaseDir()
{
	return (char *)"";
}

void BaseSystemModule::FireSignal(unsigned int signal, void *data)
{
	for (ISystemModule *mod = (ISystemModule *)m_Listener.GetFirst(); mod; mod = (ISystemModule *)m_Listener.GetNext())
	{
		mod->ReceiveSignal(this, signal, data);
	}
}
