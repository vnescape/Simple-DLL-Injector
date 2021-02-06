#include "Process.h"
#include <iostream>


int main(void) {
	Process list;
	list.GetProcessIDs();
	for (unsigned int i = 0; i < list.cProcesses; i++) {
		std::cout << "ProcessID: " << list.aProcesses[i] << std::endl;
		std::cout << "ProcessName: ";
		list.PrintProcessName(list.aProcesses[i]);
		std::cout << "\n";
	}

	std::cin.get();
	return 0;
}
