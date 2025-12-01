#include "ui_demo_window.h"
#include "imgui.h"
#include "hud.h"
#include "keydefs.h"

CImGuiDemoWindow iDemoWindow;

bool CImGuiDemoWindow::m_ShowWindow = false;

void CImGuiDemoWindow::Initialize()
{
    gEngfuncs.pfnAddCommand("ui_imgui_demo", CImGuiDemoWindow::CmdShowDemoWindow);
}

void CImGuiDemoWindow::VidInitialize()
{
}

void CImGuiDemoWindow::Terminate()
{
}

void CImGuiDemoWindow::Think()
{
}

void CImGuiDemoWindow::Draw()
{
    if (m_ShowWindow) {
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it
		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_ShowWindow);       // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider

		if (ImGui::Button("Button"))                            // Buttons return true when clicked
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

	}
}

bool CImGuiDemoWindow::Active()
{
    return m_ShowWindow;
}

bool CImGuiDemoWindow::CursorRequired()
{
    return true;
}

bool CImGuiDemoWindow::HandleKey(bool keyDown, int keyNumber, const char *bindName)
{
    if (keyNumber == K_ESCAPE)
    {
        m_ShowWindow = false;
    }
    return false;
}

void CImGuiDemoWindow::CmdShowDemoWindow()
{
    m_ShowWindow = true;
}
