add_sources(
        # base
        GameUI_Interface.cpp
        EngineInterface.h
        icvar.cpp
        icvar.h
        VGuiSystemModuleLoader.cpp
        VGuiSystemModuleLoader.h
        BasePanel.cpp
        BasePanel.h
        Sys_Utils.cpp
        Sys_Utils.h
        ModInfo.cpp
        ModInfo.h
        GameConsole.cpp
        GameConsole.h
        GameConsoleDialog.cpp
        GameConsoleDialog.h
        GameMenu.cpp
        GameMenu.h
        BackgroundMenuButon.cpp
        BackgroundMenuButon.h

        # controls
        CvarSlider.cpp
        KeyToggleCheckButton.cpp
        VControlsListPanel.cpp
        CvarToggleCheckButton.cpp
        LabeledCommandComboBox.cpp
        CvarNegateCheckButton.cpp
        CvarTextEntry.cpp
        # misc
        ../common/convar.cpp

        # options
        options/optionsdialog.cpp
        options/MultiplayerAdvancedDialog.cpp
        options/OptionsSubAdvanced.cpp
        options/OptionsSubAudio.cpp
        options/OptionsSubKeyboard.cpp
        options/OptionsSubMouse.cpp
        options/OptionsSubMultiplayer.cpp
        options/OptionsSubVideo.cpp
        options/OptionsSubVoice.cpp

        menuitems/gamemenuitem.cpp
        menuitems/gamemenuitem.h
        menuitems/gameuimenubutton.cpp
        menuitems/gameuimenubutton.h
)
