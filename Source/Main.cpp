#include "Application.h"

#include <stdlib.h>

#ifndef _WIN64
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#else
#pragma comment( lib, "SDL/libx64/SDL2.lib" )
#pragma comment( lib, "SDL/libx64/SDL2main.lib" )
#endif

enum class main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = nullptr;

int main(int argc, char** argv)
{
	LOG("--- Starting Davos2D Engine from[%s]", argv[0], 'i');

	int main_return = EXIT_FAILURE;
	main_states state = main_states::MAIN_CREATION;

	while (state != main_states::MAIN_EXIT)
	{
		switch (state)
		{
		case main_states::MAIN_CREATION:

			LOG("-------------- Application Creation --------------", 'i');
			App = new Application();
			state = main_states::MAIN_START;
			break;

		case main_states::MAIN_START:

			LOG("-------------- Application Init --------------", 'i');
			if (App->Init())
			{
				LOG("-------------- Application Update --------------", 'i');
				state = main_states::MAIN_UPDATE;
			}
			else
			{
				LOG("Application Init exits with ERROR", 'e');
				state = main_states::MAIN_EXIT;
			}
			break;

		case main_states::MAIN_UPDATE:
			if (App->Update() == false)
				state = main_states::MAIN_FINISH;
			break;

		case main_states::MAIN_FINISH:

			LOG("-------------- Application CleanUp --------------", 'i');
			if (App->CleanUp())
				main_return = EXIT_SUCCESS;
			else
				LOG("Application CleanUp exits with ERROR", 'e');

			state = main_states::MAIN_EXIT;
			break;
		}
	}

	RELEASE(App);
	LOG("--- Exiting Engine ---", 'i');

	return main_return;
}
