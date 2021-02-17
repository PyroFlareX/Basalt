//#include "Vulkan.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

#include "Engine/Engine.h"


//JobSystem jobSystem;

void print(Job job)
{
	for (int i = 0; i < 100; ++i)
	{
		//std::cout << i << std::endl;
	}
	std::cout << "Job done\n";
	return;
}


int main() {

	/*Job j = jobSystem.createJob(&print);

	for(int i = 0; i < 10; ++i)
	{
		jobSystem.schedule(j);
	}
	jobSystem.wait();*/

	vn::Context context;
	vn::Device device;

	context.setDeviceptr(&device);
	context.initAPI();
	
	while (context.isOpen())
	{
		context.clear();




		context.update();

	}

	/*Application app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}*/

	return EXIT_SUCCESS;
}