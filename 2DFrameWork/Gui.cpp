#include "Framework.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Gui::MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam);
}

void Gui::SetNavInput(DeviceState deviceState, ImGuiNavInput navInput)
{
	ImGui::GetIO().NavInputs[navInput] = 1.0f;
	switch (deviceState)
	{
	case DeviceState::KEYBOARD: ImGui::GetCurrentContext()->NavInputSource = ImGuiInputSource_Keyboard; break;
	case DeviceState::CONTROLLER: ImGui::GetCurrentContext()->NavInputSource = ImGuiInputSource_Gamepad; break;
	default: assert(false);
	}
}

Gui::Gui()
{
	fileID = 0;

	target = nullptr;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsResizeFromEdges = true;
	//io.FontAllowUserScaling = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	if(!App.is_game) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	io.ConfigViewportsNoDefaultParent = true;
	io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.
	ImFontConfig korConfig;
	korConfig.OversampleH = korConfig.OversampleV = 1;
	korConfig.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x3131, 0x3163, // Korean alphabets
		0xAC00, 0xD7A3, // Korean characters
		0xFFFD, 0xFFFD, // Invalid
		0x25A1, 0x25CB,	// shapes
		0x2190, 0x2193,	// arrows
		0,
	};
	smallFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Contents/Fonts/aGothic14.ttf", 15.0f, &korConfig, ranges);
	bigFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Contents/Fonts/aGothic14.ttf", 17.0f, &korConfig, ranges);
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(App.handle);
	ImGui_ImplDX11_Init(D3D->GetDevice(), D3D->GetDC());


	ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
	ImGui::StyleColorsDark();
	//ImGuiStyle& style = ImGui::GetStyle();


}

Gui::~Gui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Gui::ResizeScreen()
{
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}


void Gui::Update()
{
	fileID = 0;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::GetIO().AddKeyEvent(ImGuiKey_Space, INPUT->KeyPress(VK_RETURN));
	ImGui::NewFrame();
	ImGui::SetCurrentFont(smallFont);
}

void Gui::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Gui::DebugRender()
{
	if (!VAR->isDebug) return;
	ImGui::Begin("Detail");
	if (target)
		target->RenderDetail();
	ImGui::End();
}

bool Gui::FileImGui(string button, string name, string fiter, string minPath)
{
	string strId = to_string(fileID);
	fileID++;

	// open Dialog Simple
	if (ImGui::Button(button.c_str()))
		ImGuiFileDialog::Instance()->OpenDialog(strId, name.c_str(), fiter.c_str(), minPath, ".");

	// display
	if (ImGuiFileDialog::Instance()->Display(strId))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			ImGuiFileDialog::Instance()->Close();
			return true;
			// action
		}
		// close
		ImGuiFileDialog::Instance()->Close();
	}
	return false;
}

void Gui::TextCentered(string text)
{
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

void Gui::TextWithPadding(string text, float width, EnumTextAlign align)
{
	// 양쪽 padding까지 포함해 width만큼의 크기를 가진 text 생성
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
	float padding_left = 0.0f;
	float padding_right = 0.0f;
	if (width > textWidth)
	{
		switch (align)
		{
		case EnumTextAlignLeading: padding_right = width - textWidth; break;
		case EnumTextAlignTrailing: padding_left = width - textWidth; break;
		case EnumTextAlignCenter: padding_left = padding_right = (width - textWidth) / 2.0f; break;
		}
	}
	if (padding_left > 0.0f)
	{
		ImGui::Dummy({ padding_left,0.0f });
		ImGui::SameLine();
	}
	ImGui::Text(text.c_str());
	if (padding_right > 0.0f)
	{
		ImGui::SameLine();
		ImGui::Dummy({ padding_right,0.0f });
	}
}

