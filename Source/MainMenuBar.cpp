#include "MainMenuBar.h"

#include "Utils.h"
#include "MessageBus.h"

#include "Application.h" //***FPS IN BAR

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

void MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		File();
		Edit();
		GameObjects();
		Tools();
		Window();
		Help();

		// Show FPS
		ImGui::Text("FPS:%i", App->GetFPS());

		ImGui::EndMainMenuBar();
	}
}

// ---------------------------------------------
void MainMenuBar::File()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New", "Ctrl+N", false))
		{
		}
		if (ImGui::MenuItem("Open", "Ctrl+O", false))
		{
		}
		if (ImGui::MenuItem("Save", "Ctrl+S"))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Build", "Ctrl+S"))
		{
		}
		if (ImGui::MenuItem("Preferences")) // --- Open Panel Configuration
		{
			App->message->Publish(new OnOpenPanelConfiguration());
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Quit", "ESC"))
		{
		}
		ImGui::EndMenu();
	}
}

void MainMenuBar::Edit()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false))
		{
		}
		if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Import", "Ctrl+I", false, false))
		{
		}
		if (ImGui::MenuItem("Export", "Ctrl+E", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Cut", "Ctrl+X", false, false))
		{
		}
		if (ImGui::MenuItem("Copy", "Ctrl+C", false, false))
		{
		}
		if (ImGui::MenuItem("Paste", "Ctrl+V", false, false))
		{
		}
		ImGui::Separator();

		if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, false))
		{
		}
		if (ImGui::MenuItem("Delete", "Supr", false, false))
		{
		}
		ImGui::EndMenu();
	}
}

void MainMenuBar::GameObjects()
{
	if (ImGui::BeginMenu("GameObjects"))
	{
		ImGui::EndMenu();
	}
}

void MainMenuBar::Tools()
{
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Map Editor", NULL, false, false))
		{
		}
		if (ImGui::MenuItem("Audio Editor", NULL, false, false))
		{
		}
		if (ImGui::MenuItem("Animations", NULL, false, false))
		{
		}
		if (ImGui::MenuItem("Particles", NULL, false, false))
		{
		}
		if (ImGui::MenuItem("Lights", NULL, false, false))
		{
		}
		if (ImGui::MenuItem("Shaders", NULL, false, false))
		{
		}
		ImGui::EndMenu();
	}
}

void MainMenuBar::Window()
{
	if (ImGui::BeginMenu("Window"))
	{
		//ImGui::MenuItem("Console", NULL, &PanelConsole->active);
		//ImGui::MenuItem("Hierarchy", NULL, &GetPanel(3)->active);
		//ImGui::MenuItem("Assets", NULL, &GetPanel(4)->active);
		//ImGui::MenuItem("Resources", NULL, &GetPanel(5)->active);
		//ImGui::MenuItem("Scene", NULL, &GetPanel(6)->active);
		//ImGui::MenuItem("Game", NULL, &GetPanel(7)->active);
		//ImGui::MenuItem("Inspector", NULL, &GetPanel(8)->active);

		ImGui::EndMenu();
	}
}

void MainMenuBar::Help()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Github"))			Utils::Link("https://github.com/DavidTello1/Motor2D");
		if (ImGui::MenuItem("Documentation"))	Utils::Link("https://github.com/DavidTello1/Motor2D/wiki");
		if (ImGui::MenuItem("Latest Version"))	Utils::Link("https://github.com/DavidTello1/Motor2D/releases");
		if (ImGui::MenuItem("Report a bug"))	Utils::Link("https://github.com/DavidTello1/Motor2D/issues");
		ImGui::EndMenu();
	}
}
