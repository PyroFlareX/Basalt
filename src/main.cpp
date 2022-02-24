#include <stdexcept>
#include <iostream>
#include "Application.h"

int main() 
{
	Application app;

	try 
	{
		app.RunLoop();
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "There was an uncaught exception thrown!\n";
		return 1;
	}

	return 0;
}