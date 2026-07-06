"Resource/UI/FreezePanel_Basic.res"
{	
	"HudFreezePanel"
	{
		"ControlName"	"HudFreezePanel"
		"xpos"			"c-110"
		"ypos"			"c"		// was 290 in valve's mockups; placing up high to avoid overlap with win panel and achievements
		"zpos"			"0"		
		"wide"			"220"
		"tall"			"70"
		"visible"		"1"
		"bgcolor_override" "0 0 0 192"
		"fgcolor_override" "251 176 59 255"
		
		"KillerName"
		{	
			"ControlName"	"Label"
			"font"			"UiBold"
			"xpos"			"72"	// 64+8
			"ypos"			"4"
			"zpos"			"3"
			"wide"			"140"
			"tall"			"15"
			"autoResize"	"1"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"%killername%"
			"textAlignment"		"south"
			"fgcolor_override"	"204 204 204 255"
		}

		"InfoLabel1"
		{	
			"ControlName"	"Label"
			"font"			"UiBold"
			"xpos"			"72"	// (220-100)/2
			"ypos"			"24"
			"zpos"			"2"
			"wide"			"100"
			"tall"			"10"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"%InfoLabel1%"
			"textAlignment"	"center"
			"fgcolor_override"	"184 0 0 255"
		}
		
		// Temp disable cuz lack of dominaions
		"InfoLabel2"
		{	
			"ControlName"	"Label"
			"font"			"UiBold"
			"xpos"			"60"	// (212-100)/2
			"ypos"			"26"
			"zpos"			"3"
			"wide"			"100"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"0"
			"enabled"		"1"
			"labelText"		"%InfoLabel2%"
			"textAlignment"	"north"
			"fgcolor_override"	"184 0 0 255"
		}
		
		"ScreenshotLabel"
		{
			"ControlName"	"Label"
			"font"			"Default"
			"xpos"			"72"	// (212-100)/2
			"ypos"			"45"
			"zpos"			"2"
			"wide"			"120"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"#FreezePanel_ScreenShot"
			"textAlignment"	"north"
			"fgcolor_override"	"184 0 0 255"
		}
		
		//"AvatarGlow"
		//{
		//	"ControlName"	"ImagePanel"
		//	"xpos"			"-4"
		//	"ypos"			"-5"	// (54-64)/2
		//	"zpos"			"1"
		//	"wide"			"64"
		//	"tall"			"64"
		//	"visible"		"1"
		//	"enabled"		"1"
		//	"image"			"../hud/avatar_glow_64"
		//	"drawcolor"		"100 100 100 255"
		//	"scaleImage"	"1"
		//}

		"AvatarImage"
		{
			"ControlName"	"ImagePanel"
			"xpos"			"0"
			"ypos"			"0"	// (54-32)/2
			"zpos"			"4"
			"wide"			"64"
			"tall"			"64"
			"visible"		"1"
			"enabled"		"1"
			"scaleImage"	"0"
		}

		// Commented to future
		//"DominationIcon"
		//{
		//	"ControlName"	"ImagePanel"
		//	"xpos"			"172"
		//	"ypos"			"9"	// (54-36)/2
		//	"zpos"			"2"
		//	"wide"			"36"
		//	"tall"			"36"
		//	"visible"		"1"
		//	"enabled"		"1"
		//	"image"			"../hud/freeze_nemesis"
		//	"scaleImage"	"1"
		//}
		
		"KillerHealth"
		{
			"ControlName"	"HorizontalGauge"
			"xpos"			"0"
			"ypos"			"64"
			"zpos"			"2"
			"wide"			"64"
			"tall"			"6"
			"visible"		"1"
			"enabled"		"1"
		}	
	}
}
