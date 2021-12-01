#include <stdexcept>
#include "Application.h"

int main() 
{
	Application app;

	try {
		app.RunLoop();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return 1;
	}

	//std::cout << "End program\n";

	return 0;
}