//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef OPTIONSSUBMULTIPLAYER_H
#define OPTIONSSUBMULTIPLAYER_H
#include <cstdint>
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/BitmapImagePanel.h>
class CLabeledCommandComboBox;
// class CBitmapImagePanel;

class CCvarToggleCheckButton;
class CCvarTextEntry;
class CCvarSlider;
class CMultiplayerAdvancedDialog;

//-----------------------------------------------------------------------------
// Purpose: multiplayer options property page
//-----------------------------------------------------------------------------
class COptionsSubMultiplayer : public vgui2::PropertyPage
{
public:
    COptionsSubMultiplayer(vgui2::Panel *parent);
    ~COptionsSubMultiplayer();
	DECLARE_CLASS_SIMPLE(COptionsSubMultiplayer, vgui2::PropertyPage);

protected:
    virtual void OnCommand(const char *command);

    // Called when page is loaded.  Data should be reloaded from document into controls.
    virtual void OnResetData();
    // Called when the OK / Apply button is pressed.  Changed data should be written into document.
    virtual void OnApplyChanges();

    DECLARE_PANELMAP();

private:
	MESSAGE_FUNC(OnSliderMoved, "SliderMoved");
	// MESSAGE_FUNC(OnTextChanged, "TextChanged");
	// typedef vgui2::PropertyPage BaseClass;

	void InitModelList(CLabeledCommandComboBox *cb);
	void InitLogoList(CLabeledCommandComboBox *cb);

	void RemapModel();
    void RemapLogo();

	void OnTextChanged(vgui2::Panel *panel);
	// void OnSliderMoved(KeyValues *data);
	void OnApplyButtonEnable();

	void RemapPalette(char *filename, int topcolor, int bottomcolor);
    void RemapLogoPalette(char *filename, int r, int g, int b);

    void ColorForName(char const *pszColorName, int &r, int &g, int &b);

    vgui2::CBitmapImagePanel *m_pModelImage;
    CLabeledCommandComboBox *m_pModelList;

    vgui2::CBitmapImagePanel *m_pLogoImage;
    CLabeledCommandComboBox *m_pLogoList;
    char m_LogoName[32];

    CLabeledCommandComboBox	*m_pColorList;

    CCvarTextEntry *m_pNameTextEntry;
    CCvarSlider *m_pPrimaryColorSlider;
    CCvarSlider *m_pSecondaryColorSlider;
    CCvarToggleCheckButton *m_pHighQualityModelCheckBox;

    int	m_nTopColor;
    int	m_nBottomColor;

    int	m_nLogoR;
    int	m_nLogoG;
    int	m_nLogoB;

    vgui2::DHANDLE<CMultiplayerAdvancedDialog> m_hMultiplayerAdvancedDialog;
};

#pragma pack(push, 1)

struct BMP_FILEHEADER
{
	uint16 bfType;
	uint32 bfSize;
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;
};

struct BMP_RGBQUAD
{
	byte rgbBlue;
	byte rgbGreen;
	byte rgbRed;
	byte rgbReserved;
};

struct BMP_RGBTRIPLE
{
	byte rgbtBlue;
	byte rgbtGreen;
	byte rgbtRed;
};

#pragma pack(pop)

//const uint16 DIB_HEADER_MARKER = 0x4D42; // 'BM'

#endif // OPTIONSSUBMULTIPLAYER_H
