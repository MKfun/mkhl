#ifndef KISAKSTRIKE_RKHUD_INFOBAR_H
#define KISAKSTRIKE_RKHUD_INFOBAR_H
#include <rmlui/rocketuiimpl.h>
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/DataModelHandle.h>
#pragma pop_macro("Assert")


class RkHudInfoBar {
public:
    explicit RkHudInfoBar(const char *value);
    virtual ~RkHudInfoBar();
    static RkHudInfoBar m_Instance;
    // Overrides from CHudElement
    void LevelInit(void);
    virtual void LevelShutdown(void);
    virtual void SetActive(bool bActive);
    virtual bool ShouldDraw(void);
    void ShowPanel(bool bShow, bool force);

    static void UnloadRkInfoBar();
    static void LoadRkInfoBar();
	Rml::ElementDocument *m_pInstance = nullptr;
	bool		m_bVisible;
	Rml::DataModelHandle m_dataModel = nullptr;
	static struct ReloadDocumentFuncs
    {
        void *UnloadRkInfoBar();
        void *LoadRkInfoBar();
    } reloadDocumentFuncs;
    static struct InfoBarData
    {
        int hp;
        int armor;
        bool hasHelmet;
        int ammo;
        int ammoReserve;
        Rml::String fireModeString;
        Rml::String primaryString;
        Rml::String secondaryString;
        Rml::String knifeString;
		bool hasSecondary;
		int col_r;
		int col_g;
		int col_b;
		int numKills;
	} infoBarData;
	void UpdateHealth(int new_hp);
};

#endif //KISAKSTRIKE_RKHUD_INFOBAR_H
