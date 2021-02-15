#include "Vulkan.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "Engine/Engine.h"


JobSystem jobSystem;

int main() {


	


	Application app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}