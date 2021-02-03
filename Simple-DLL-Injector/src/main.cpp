#include <iostream>
#include <thread>
#include <chrono>
#include "Process.h"

int main(void) {

	PrintProcessID();

	/*for (;;) {
		std::cout << "Hello DLL World" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}*/

	return 0;
}