set(GAMEUI_SOURCES
    "../common/GameUI/ObjectList.cpp"
    "../common/GameUI/Random.cpp"
    "../../source_sdk/tier1/checksum_md5.cpp"
    "../public/convar.cpp"
    "../../source_sdk/tier1/interface.cpp"
    "../../source_sdk/tier1/mempool.cpp"
    "../../source_sdk/tier1/utlbuffer.cpp"
    "../../source_sdk/tier1/utlsymbol.cpp"
    # "../Tracker/common/msgbuffer.cpp"
    # "../Tracker/common/netapi.cpp"
    # "../Tracker/common/Socket.cpp"
    "BackgroundMenuButton.cpp"
    "BasePanel.cpp"
    "BitmapImagePanel.cpp"
    "ChangeGameDialog.cpp"
    "CommandCheckButton.cpp"
    "ContentControlDialog.cpp"
    "CreateMultiplayerGameBotPage.cpp"
    "CreateMultiplayerGameDialog.cpp"
    "CreateMultiplayerGameGameplayPage.cpp"
    "CreateMultiplayerGameServerPage.cpp"
    "CvarNegateCheckButton.cpp"
    "CvarSlider.cpp"
    "CvarTextEntry.cpp"
    "CvarToggleCheckButton.cpp"
    "DemoEditDialog.cpp"
    "DemoEventsDialog.cpp"
    "DemoPlayerDialog.cpp"
    "DemoPlayerFileDialog.cpp"
    "GameApp.cpp"
    "GameConsole.cpp"
    "GameConsoleDialog.cpp"
    "GameMenuButton.cpp"
    "GameUI_Interface.cpp"
    "HelpDialog.cpp"
    "KeyToggleCheckButton.cpp"
    "LabeledComboBox.cpp"
    "LabeledCommandComboBox.cpp"
    "LoadGameDialog.cpp"
    "LoadingDialog.cpp"
    "LogoFile.cpp"
    "ModInfo.cpp"
    "MultiplayerAdvancedDialog.cpp"
    "MultiplayerCustomizeDialog.cpp"
    "NewGameDialog.cpp"
    "OptionsDialog.cpp"
    "OptionsSubAdvanced.cpp"
    "OptionsSubAudio.cpp"
    "OptionsSubKeyboard.cpp"
    "OptionsSubMouse.cpp"
    "OptionsSubMultiplayer.cpp"
    "OptionsSubVideo.cpp"
    "OptionsSubVoice.cpp"
    "PanelListPanel.cpp"
    "PlayerListDialog.cpp"
    "RunGameEngine.cpp"
    "SaveGameDialog.cpp"
    "ScriptObject.cpp"
    "SteamPasswordDialog.cpp"
    "Sys_Utils.cpp"
    "Taskbar.cpp"
    "TaskButton.cpp"
    "vcontrolslistpanel.cpp"
    "VGuiSystemModuleLoader.cpp"
)

add_library(gameui SHARED ${GAMEUI_SOURCES})

# Include directories
target_include_directories( gameui PRIVATE
    ..
    ../public
    ../common
    ../../source_sdk/public/tier0
    ../../source_sdk/public/tier1
    ../../source_sdk/public
    ../../source_sdk/public/mathlib
    ../../source_sdk/common
    ../../source_sdk
    ../pm_shared

    .
)

target_compile_definitions( gameui PRIVATE
    _LINUX
    POSIX
    GNUC
    # _CRT_NONSTDC_NO_DEPRECATE
    # _MBCS
    _USRDLL
    GAMEUI_EXPORTS
    NO_MALLOC_OVERRIDE
    SOURCE_SDK_GFX_PATH="ui/gfx/vgui"

)
target_link_libraries(gameui PRIVATE
    "../lib/public/*.lib"
)
