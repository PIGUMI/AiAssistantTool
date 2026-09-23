#include <Window.h>
#include <iostream>
#include "Controller.h"


int main()
{
	SetConsoleOutputCP(CP_UTF8);

	Controller* controller = new Controller();

	while (true)
	{
		std::string input;


		std::cin >> input;

		std::string response = controller->Run(input);

		std::cout << "Response: " << response << std::endl;

	}

	delete controller;
	controller = nullptr;

	return 0;

}