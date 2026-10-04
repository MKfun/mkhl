#ifndef RKHUD_ELEM_INTERFACE_H
#define RKHUD_ELEM_INTERFACE_H

class CRocketHudElem
{
public:
	CRocketHudElem() { };
	virtual void LevelInit(void) = 0;
	virtual void LevelShutdown(void) = 0;
	virtual void SetActive(bool bActive) = 0;
	virtual bool ShouldDraw(void) = 0;
	virtual void ShowPanel(bool bShow, bool force) = 0;
	virtual void Update(void) {};
};

#endif // RKHUD_ELEM_INTERFACE_H
