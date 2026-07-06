// Source scheme for VGUI2 applications and tools
// (c) Klapan Software 2025
Scheme
{
	Colors
	{
		// base colors
		None							"0 0 0 0"
		Black							"0 0 0 255"
		White							"255 255 255 255"
		Grey							"142 152 167 255"
		Dark							"28 33 42 255"
		AlmostBlack						"22 25 28 255"

		// control colors
		BorderColor						"82 95 122 255"
		DarkCorner						"58 68 88 255"

		ButtonFace						"98 108 119 20"
		ButtonFaceDisabled				"86 93 106 15"
		ButtonFaceActive				"86 93 106 240"
		ButtonBorder					"75 84 102 255"
		ButtonBorderFocus				"125 133 148 255"

		Text							"200 208 220 255"
		Text2							"176 186 196 255"
		TextDisabled					"102 106 112 255"
		TextHover						"213 217 234 255"
		TextSelected					"228 233 240 255"
		TextSelectedBG					"73 86 110 255"

		Label							"158 172 179 255"
		LabelDisabled					"109 115 126 255"
		LabelFocus						"197 203 216 255"

		ScrollBG						"53 59 66 255"

		TitleBar						"21 24 31 80"
		TitleBarFocus					"14 31 56 80"

		DialogBG						"42 46 51 255"

		PropertySheetBG					"50 55 61 255"

		MenuBG							"56 60 68 255"

		Hightlight5						"24 53 82 255"
		Hightlight5a					"30 66 102 255"
		Hightlight5b					"17 42 86 255"

				"BaseText"			"216 222 211 255"	// used in text windows, lists
		"BrightBaseText"	"255 255 255 255"	// brightest text
		"SelectedText"		"255 255 255 255"	// selected text
		"DimBaseText"		"160 170 149 255"	// dim base text
		"LabelDimText"		"160 170 149 255"	// used for info text
		"ControlText"		"216 222 211 255"	// used in all text controls
		"BrightControlText"	"196 181 80 255"	// use for selected controls
		"DisabledText1"		"117 128 111 255"	// disabled text
		"DisabledText2"		"40 46 34 255"		// overlay color for disabled text (to give that inset look)
		"DimListText"		"117 134 102 255"	// offline friends, unsubscribed games, etc.

		// background colors
		"ControlBG"			"90 106 80 124"		// background color of controls
		"ControlDarkBG"		"90 106 80 124"		// darker background color; used for background of scrollbars
		"WindowBG"			"0 0 0 124"		// background color of text edit panes (chat, text entries, etc.)
		"SelectionBG"		"149 136 49 124"	// background color of any selected text or menu item
		"SelectionBG2"		"40 46 34 124"		// selection background in window w/o focus
		"ListBG"			"62 70 55 124"		// background of server browser, buddy list, etc.

		// titlebar colors
		"TitleText"			"255 255 255 124"
		"TitleDimText"		"255 0 0 124"
		"TitleBG"			"255 0 0 124"
		"TitleDimBG"		"255 0 0 124"

		// slider tick colors
		"SliderTickColor"	"127 140 127 255"
		"SliderTrackColor"	"31 31 31 255"

		// border colors
		"BorderBright"		"0 255 0 255"	// the lit side of a control
		"BorderDark"		"0 255 0 255"		// the dark/unlit side of a control
		"BorderSelection"	"0 255 0 255"			// the additional border color for displaying the default/selected button

		"team0"				"125 165 210"
		"team1"				"200 90 70"
		"team2"				"225 205 45"
		"team3"				"145 215 140"
		"team4"				"255 170 0"

		// Old
		"OffWhite"			"221 221 221 255"
		"DullWhite"			"190 190 190 255"
		"Orange"			"255 155 0 255"
		"TransparentBlack"	"0 0 0 128"
		"Blank"				"0 0 0 0"
		"SteamLightGreen"	"157 194 80 255"
		"AchievementsLightGrey"		"79 79 79 255"
		"AchievementsDarkGrey"		"55 55 55 255"
		"AchievementsInactiveFG"	"130 130 130 255"
	}

	// Base settings for all panels
	BaseSettings
	{
		Button.TextColor				Text
		Button.BgColor					None

		Button.ArmedTextColor			Text2
		Button.ArmedBgColor				"Blank"

		Button.DepressedTextColor		Text2
		Button.DepressedBgColor			"86 93 106 240"
		Button.FocusBorderColor			"125 133 148 255"

		CheckButton.TextColor			Label
		CheckButton.SelectedTextColor	Text
		CheckButton.BgColor				None
		CheckButton.Border1  			PropertySheetBG // DepressedBorder 		// the left checkbutton border
		CheckButton.Border2  			"BorderColor"		// the right checkbutton border
		CheckButton.Check				Label	// color of the check itself
		CheckButton.HighlightFgColor		"OffWhite"
		CheckButton.ArmedBgColor		"Blank"
		CheckButton.DepressedBgColor		"Blank"
		CheckButton.DisabledBgColor	   	"TransparentBlack"

		ComboBoxButton.ArrowColor		TextEntrySelected
		ComboBoxButton.ArmedArrowColor	TextEntrySelected
		ComboBoxButton.BgColor			ButtonBorder
		ComboBoxButton.DisabledBgColor	ButtonFaceDisabled

		Frame.TitleTextInsetX			16
		Frame.ClientInsetX				8
		Frame.ClientInsetY				6
		Frame.BgColor					DialogBG
		Frame.OutOfFocusBgColor			DialogBG
		Frame.FocusTransitionEffectTime	"0.3"							// time it takes for a window to fade in/out on focus/out of focus
		Frame.TransitionEffectTime		"0.3"
		Frame.AutoSnapRange				"0"
		FrameGrip.Color1				"200 200 200 196"
		FrameGrip.Color2				"0 0 0 196"
		FrameTitleButton.FgColor		"200 200 200 196"
		FrameTitleButton.BgColor		"Blank"
		FrameTitleButton.DisabledFgColor	"255 255 255 192"
		FrameTitleButton.DisabledBgColor	"Blank"
		FrameSystemButton.FgColor		"Blank"
		FrameSystemButton.BgColor		"Blank"
		FrameSystemButton.Icon			""
		FrameSystemButton.DisabledIcon	""
		FrameTitleBar.Font				"DefaultLarge"
		FrameTitleBar.TextColor			ButtonBorderFocus
		FrameTitleBar.BgColor			TitleBarFocus
		FrameTitleBar.DisabledTextColor	ButtonBorderFocus
		FrameTitleBar.DisabledBgColor	None

		GraphPanel.FgColor				"White"
		GraphPanel.BgColor				"TransparentBlack"

		Label.TextDullColor				"DullWhite"
		Label.TextColor					Text
		Label.TextBrightColor			White
		Label.SelectedTextColor			White
		Label.BgColor					None
		Label.DisabledFgColor1			LabelDisabled
		Label.DisabledFgColor2			None

		ListPanel.TextColor					Text
		ListPanel.TextBgColor				"Blank"
		ListPanel.BgColor					AlmostBlack
		ListPanel.SelectedTextColor			TextEntrySelected
		ListPanel.SelectedBgColor			TextSelectedBG
		ListPanel.SelectedOutOfFocusBgColor	DialogBG
		ListPanel.EmptyListInfoTextColor	Label

		Menu.TextColor					Text
		Menu.BgColor					MenuBG
		Menu.ArmedTextColor				ButtonBorder
		Menu.ArmedBgColor				TextSelected
		Menu.TextInset					12 // 16

		Panel.FgColor					Text
		Panel.BgColor					None

		ProgressBar.FgColor				"White"
		ProgressBar.BgColor				"TransparentBlack"

		PropertySheet.TextColor			"OffWhite"
		PropertySheet.SelectedTextColor	"White"
		PropertySheet.TransitionEffectTime	"0.25"	// time to change from one tab to another

		RadioButton.TextColor			"OffWhite"
		RadioButton.SelectedTextColor	"White"

		RichText.TextColor				Text
		RichText.BgColor				DialogBG
		RichText.SelectedTextColor		TextSelected
		RichText.SelectedBgColor		TextSelectedBG

		ScrollBar.Wide					16

		ScrollBarButton.FgColor				White
		ScrollBarButton.BgColor				None
		ScrollBarButton.ArmedFgColor		White
		ScrollBarButton.ArmedBgColor		None
		ScrollBarButton.DepressedFgColor	White
		ScrollBarButton.DepressedBgColor	None

		ScrollBarSlider.FgColor				ButtonFaceActive
		ScrollBarSlider.BgColor				ScrollBG

		SectionedListPanel.HeaderTextColor	"White"
		SectionedListPanel.HeaderBgColor	"Blank"
		SectionedListPanel.DividerColor		"Black"
		SectionedListPanel.TextColor		"DullWhite"
		SectionedListPanel.BrightTextColor	"White"
		SectionedListPanel.BgColor			PropertySheetBG
		SectionedListPanel.SelectedTextColor			"Black"
		SectionedListPanel.SelectedBgColor				"Orange"
		SectionedListPanel.OutOfFocusSelectedTextColor	"Black"
		SectionedListPanel.OutOfFocusSelectedBgColor	"255 155 0 128"

		Slider.NobColor				ButtonFaceActive
		Slider.TextColor			ButtonFaceActive
		Slider.TrackColor			ScrollBG
		Slider.DisabledTextColor1	ButtonFaceDisabled
		Slider.DisabledTextColor2	ButtonFaceDisabled

		TextEntry.TextColor			Text
		TextEntry.BgColor			ButtonBorder
		TextEntry.CursorColor		Text
		TextEntry.DisabledTextColor	TextDisabled
		TextEntry.DisabledBgColor	ButtonFaceDisabled
		TextEntry.SelectedTextColor	White
		TextEntry.SelectedBgColor	TextHover
		TextEntry.OutOfFocusSelectedBgColor	ButtonBorder
		TextEntry.FocusEdgeColor	ButtonBorderFocus

		ToggleButton.SelectedTextColor	TextHover

		Tooltip.TextColor			Text
		Tooltip.BgColor				MenuBG

		TreeView.BgColor			None
		WizardSubPanel.BgColor		None
// LEGACY PART FOR OLD CONTROLS
		"FgColor"			"Text"
		"BgColor"			"Frame.BgColor"
		"LabelBgColor"		"Label.BgColor"
		"SubPanelBgColor"	"Frame.BgColor"

		"DisabledFgColor1"		"Label.DisabledFgColor1"
		"DisabledFgColor2"		"Label.DisabledFgColor2"			// set this to the BgColor if you don't want it to draw

		"TitleBarFgColor"			"FrameTitleBar.TextColor"
		"TitleBarDisabledFgColor"	"FrameTitleBar.DisabledTextColor"
		"TitleBarBgColor"			"FrameTitleBar.BgColor"
		"TitleBarDisabledBgColor"	"FrameTitleBar.DisabledBgColor"

		"TitleBarIcon"				"resource/icon_steam"
		"TitleBarDisabledIcon"		"resource/icon_steam_disabled"

		"TitleButtonFgColor"			"FrameSystemButton.FgColor"
		"TitleButtonBgColor"			"FrameSystemButton.BgColor"
		"TitleButtonDisabledFgColor"	"FrameSystemButton.FgColor"
		"TitleButtonDisabledBgColor"	"FrameSystemButton.BgColor"

		"TextCursorColor"			"TextEntry.CursorColor"			// color of the blinking text cursor in text entries
		"URLTextColor"				"BrightBaseText"	// color that URL's show up in chat window

		Menu
		{
			"FgColor"			"Menu.TextColor"
			"BgColor"			"Menu.BgColor"
			"ArmedFgColor"		"Menu.ArmedTextColor"
			"ArmedBgColor"		"Menu.ArmedBgColor"
			"DividerColor"		"BorderDark"

			"TextInset"			"Menu.TextInset"
		}

		MenuButton	  // the little arrow on the side of boxes that triggers drop down menus
		{
			"ButtonArrowColor"	"DimBaseText"		// color of arrows
		   	"ButtonBgColor"		"WindowBG"			// bg color of button. same as background color of text edit panes
			"ArmedArrowColor"	"BrightBaseText"	// color of arrow when mouse is over button
			"ArmedBgColor"		"DimBaseText"		// bg color of button when mouse is over button
		}

		Slider
		{
			"SliderFgColor"		"Slider.NobColor"			// handle with which the slider is grabbed
			"SliderBgColor"		"Slider.TrackColor"		// area behind handle
		}

		ScrollBarSlider
		{
			"BgColor"					"ScrollBarSlider.BgColor"		// this isn't really used
			"ScrollBarSliderFgColor"	"ScrollBarSlider.BgColor"		// handle with which the slider is grabbed
			"ScrollBarSliderBgColor"	"ScrollBarSlider.BgColor"	// area behind handle
			"ButtonFgColor"				"ScrollBarSlider.FgColor"	// color of arrows
		}


		// text edit windows
		"WindowFgColor"				"BaseText"		// off-white
		"WindowBgColor"				"TextEntry.BgColor"		// redundant. can we get rid of WindowBgColor and just use WindowBG?
		"WindowDisabledFgColor"		"TextEntry.DisabledTextColor"
		"WindowDisabledBgColor"		"TextEntry.DisabledBgColor"		// background of chat conversation
		"SelectionFgColor"			"Label.SelectedTextColor"	// fg color of selected text
		"SelectionBgColor"			"ListPanel.SelectedBgColor"
		"ListSelectionFgColor"		"ListPanel.SelectedTextColor"
		"ListBgColor"				"ListPanel.BgColor"		// background of server browser control, etc
		"BuddyListBgColor"			"TextEntry.DisabledBgColor"		// background of buddy list pane

		// App-specific stuff
		"ChatBgColor"				"WindowBG"

		// status selection
		"StatusSelectFgColor"		"BrightBaseText"
		"StatusSelectFgColor2"		"BrightControlText"	// this is the color of the friends status

		// checkboxes
		"CheckButtonBorder1"   		"CheckButton.Border1"		// the left checkbutton border
		"CheckButtonBorder2"   		"CheckButton.Border2"		// the right checkbutton border
		"CheckButtonCheck"			"CheckButton.Check"	// color of the check itself
		"CheckBgColor"				"CheckButton.BgColor"

		// buttons (default fg/bg colors are used if these are not set)
		"ButtonArmedFgColor"		"Button.ArmedTextColor"
		"ButtonArmedBgColor"		"Button.ArmedBgColor"
//		"ButtonDepressedFgColor"	"BrightControlText"
//		"ButtonDepressedBgColor"

		// buddy buttons
		BuddyButton
		{
			"FgColor1"				"ControlText"
			"FgColor2"				"DimListText"
			"ArmedFgColor1"			"BrightBaseText"
			"ArmedFgColor2"			"BrightBaseText"
			"ArmedBgColor"			"SelectionBG"
		}

		Chat
		{
			"TextColor"				"BrightControlText"
			"SelfTextColor"			"BaseText"
			"SeperatorTextColor"	"DimBaseText"
		}

		"SectionTextColor"		"SectionedListPanel.TextColor"	// text color for IN-GAME, ONLINE, OFFLINE sections of buddy list
		"SectionDividerColor"	"SectionedListPanel.DividerColor"		// color of line that runs under section name in buddy list
// END OF LEGACY PART
	}

	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		// fonts are used in order that they are listed
		"Default"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"14"
				"weight"	"500"
			}
		}
		"DefaultBold"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"14"
				"weight"	"1000"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"14"
				"weight"	"500"
				"underline" "1"
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"12"
				"weight"	"0"
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"12"
				"weight"	"0"
			}
		}

		"DefaultLarge"
		{
			"1"
			{
				"name"		"Arial" [!$OSX]
				"name"		"Helvetica" [$OSX]
				"tall"		"18"
				"weight"	"0"
			}
		}
		"MenuLarge"
		{
			"1"	[$OSX]
			{
				"name"		"Helvetica Bold"
				"tall"		"20"
				"antialias" "1"
			}
			"1"	[$POSIX]
			{
				"name"		"Verdana"
				"tall"		"24"
				"weight"	"600"
				"antialias" "1"
			}
			"1"	[$WINDOWS]
			{
				"name"		"Verdana"
				"tall"		"16"
				"weight"	"600"
				"antialias" "1"
			}
			"1"	[$X360]
			{
				"name"		"Verdana"
				"tall"			"14"
				"tall_hidef"	"20"
				"weight"	"1200"
				"antialias" "1"
				"outline" "1"
			}
		}
		"ConsoleText"
		{
			"1"
			{
				"name"		 "Lucida Console" [!$OSX]
				"name"		 "Lucida Console" [$X360]
				"name"		 "Verdana" [$OSX]
				"tall"		"11" [$OSX]
				"tall"		"14" [$LINUX]
				"tall"		"10"
				"weight"	"500"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
			}
		}
	}

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		BaseBorder		DepressedBorder
		ButtonBorder	RaisedBorder
		ComboBoxBorder	DepressedBorder
		FrameBorder		NullBorder
		MenuBorder		NullBorder
		BrowserBorder	DepressedBorder
		PropertySheetBorder	RaisedBorder

		NullBorder
		{
			"backgroundtype" 0
		}

		DepressedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}
		}
		RaisedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}
		}

		TitleButtonBorder
		{
			"backgroundtype" "0"
		}

		TitleButtonDisabledBorder
		{
			"backgroundtype" "0"
		}

		TitleButtonDepressedBorder
		{
			"backgroundtype" "0"
		}

		ScrollBarButtonBorder
		{
			"inset" "2 2 0 0" [!$OSX]
			"inset" "-3 2 0 0" [$OSX]
			Left
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonDepressedBorder
		{
			"inset" "2 2 0 0" [!$OSX]
			"inset" "-3 2 0 0" [$OSX]
			Left
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}
		}

		TabBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}

		}

		TabActiveBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}

		}


		ToolTipBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}
		}

		// this is the border used for default buttons (the button that gets pressed when you hit enter)
		ButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderColor"
					"offset" "0 1"
				}
			}
			Top
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderColor"
					"offset" "1 0"
				}
			}
			Right
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "DarkCorner"
					"offset" "1 0"
				}
			}
			Bottom
			{
				"1"
				{
					"color" "Border.Selection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}
		}

		ButtonDepressedBorder
		{
			"inset" "2 1 1 1"
			Left
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "DarkCorner"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderColor"
					"offset" "0 0"
				}
			}
		}
	}

	CustomFontFiles
	{
		"1"		"resource/marlett.ttf"
	}
}
