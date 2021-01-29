#include "Application.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

enum class main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char ** argv)
{
	LOG("Starting Davos Game Engine from[%s]", argv[0]);

	int main_return = EXIT_FAILURE;
	main_states state = main_states::MAIN_CREATION;

	while (state != main_states::MAIN_EXIT)
	{
		switch (state)
		{
		case main_states::MAIN_CREATION:

			LOG("-------------- Application Creation --------------");
			App = new Application();
			state = main_states::MAIN_START;
			break;

		case main_states::MAIN_START:

			LOG("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG("Application Init exits with ERROR");
				state = main_states::MAIN_EXIT;
			}
			else
			{
				state = main_states::MAIN_UPDATE;
				LOG("-------------- Application Update --------------");
			}

			break;

		case main_states::MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == false)
				state = main_states::MAIN_FINISH;
		}
		break;

		case main_states::MAIN_FINISH:

			LOG("-------------- Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = main_states::MAIN_EXIT;

			break;

		}
	}

	RELEASE(App);
	LOG("Exiting engine...");
	return main_return;
}