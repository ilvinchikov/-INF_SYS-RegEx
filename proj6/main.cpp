#include <iostream>
#include <windows.h>
#include "projclass.h" // задачи

int main(int argc, char** argv) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");

	// запуск (через конструктор)
	RegexProject project;
	
	system("pause");
	return 0;
}
