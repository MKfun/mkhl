#ifndef DEMOPLAYER_BASESYSTEMMODULE_H
#define DEMOPLAYER_BASESYSTEMMODULE_H

#include "demoplayer_ifaces.h"
#include "containers.h"

class BaseSystemModule : public virtual ISystemModule
{
public:
	BaseSystemModule();
	virtual ~BaseSystemModule();

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
	virtual char *COM_GetBaseDir() override;

	void FireSignal(unsigned int signal, void *data);

public:
	ObjectList m_Listener;
	IBaseSystem *m_System;
	int m_Serial;
	char m_Name[256];
	double m_SystemTime;
	int m_State;
};

#endif // DEMOPLAYER_BASESYSTEMMODULE_H
