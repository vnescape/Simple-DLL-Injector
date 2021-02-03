#include <iostream>
#include <thread>
#include <chrono>

int main() {

	for (;;) {
		std::cout << "Hello DLL World" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}