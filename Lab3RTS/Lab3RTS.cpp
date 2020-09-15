// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "pch.h"
#include <iostream>
#include "Lab3RTS.h"

#pragma region Semaphore handles
HANDLE semAdd;
HANDLE semAdd2;

HANDLE semDiv;
HANDLE semDiv2;

HANDLE semMul;
HANDLE semMul2;

HANDLE semSub;
HANDLE semSub2;
#pragma endregion

#pragma region Pipe handles
HANDLE wPipeAdd;
HANDLE rPipeAdd;

HANDLE wPipeDiv;
HANDLE rPipeDiv;

HANDLE wPipeMul;
HANDLE rPipeMul;

HANDLE wPipeSub;
HANDLE rPipeSub;
#pragma endregion

#pragma region Thread handles
HANDLE threadAdd;
HANDLE threadDiv;
HANDLE threadMul;
HANDLE threadSub;
#pragma endregion

#define CreateStandartSemaphore() CreateSemaphore(NULL, 0, 1, NULL)
#define Out(val) (std::cout << "Res = " << (val) << " (thread: " << _threadid << ")" << std::endl << std::endl)
#define Release(sem) ReleaseSemaphore((sem), 1, NULL)
#define Wait(sem) WaitForSingleObject((sem), INFINITE)

DWORD WINAPI addThread(LPVOID params) {
	while (true)
	{
		Wait(semAdd);
		double operand1, operand2;
		ReadFile(rPipeAdd,&operand1,sizeof(double),NULL,NULL);
		ReadFile(rPipeAdd,&operand2, sizeof(double), NULL, NULL);
		double res = operand1 + operand2;
		std::cout << "Sum: " << operand1 << "+" << operand2 << " = " << res << " (thread: "<< _threadid << ")"<<std::endl;
		WriteFile(wPipeAdd, &res, sizeof(double), NULL, NULL);
		Release(semAdd2);
	}
}
DWORD WINAPI divThread(LPVOID params) {
	while (true)
	{
		Wait(semDiv);
		double operand1, operand2;
		ReadFile(rPipeDiv, &operand1, sizeof(double), NULL, NULL);
		ReadFile(rPipeDiv, &operand2, sizeof(double), NULL, NULL);
		double res = operand1 / operand2;
		std::cout << "Div: " << operand1 << "/"<< operand2 <<" = " << res << " (thread: " << _threadid << ")" << std::endl;
		WriteFile(wPipeDiv, &res, sizeof(double), NULL, NULL);
		Release(semDiv2);
	}
}
DWORD WINAPI mulThread(LPVOID params) {
	while (true)
	{
		Wait(semMul);
		double operand1, operand2;
		ReadFile(rPipeMul, &operand1, sizeof(double), NULL, NULL);
		ReadFile(rPipeMul, &operand2, sizeof(double), NULL, NULL);
		double res = operand1 * operand2;
		std::cout << "Mul: " << operand1 << "*" << operand2 << " = " << res << " (thread: " << _threadid << ")" << std::endl;
		WriteFile(wPipeMul, &res, sizeof(double), NULL, NULL);
		Release(semMul2);;
	}
}
DWORD WINAPI subThread(LPVOID params) {
	while (true)
	{
		Wait(semSub);
		double operand1, operand2;
		ReadFile(rPipeSub, &operand1, sizeof(double), NULL, NULL);
		ReadFile(rPipeSub, &operand2, sizeof(double), NULL, NULL);
		double res = operand1 - operand2;
		std::cout << "Sub: " << operand1 << "-" << operand2 << " = " << res << " (thread: " << _threadid << ")" << std::endl;
		WriteFile(wPipeSub, &res, sizeof(double), NULL, NULL);
		Release(semSub2);
	}
}

double GetRes(HANDLE wPipe, HANDLE rPipe,HANDLE sem,HANDLE sem2, double left_operand, double right_operand) {
	double res = 0;
	WriteFile(wPipe, &left_operand, sizeof(double), NULL, NULL);
	WriteFile(wPipe, &right_operand, sizeof(double), NULL, NULL);
	ReleaseSemaphore(sem, 1, NULL);

//расчёт в другом потоке

	WaitForSingleObject(sem2, INFINITE);
	ReadFile(rPipe, &res, sizeof(double), NULL, NULL);
	return res;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	const int count = 3;
	double values[count];
	double res = 0,average = 0;

	std::cout << "Выборочная дисперсия для трех чисел\n";
    std::cout << "Введите три числа для вычисления дисперсии:\n"; 
	std::cin >> values[0] >> values[1] >> values[2];

	#pragma region Create handles
	semAdd = CreateStandartSemaphore();
	semDiv = CreateStandartSemaphore();
	semMul = CreateStandartSemaphore();
	semSub = CreateStandartSemaphore();
	semAdd2 = CreateStandartSemaphore();
	semDiv2 = CreateStandartSemaphore();
	semMul2 = CreateStandartSemaphore();
	semSub2 = CreateStandartSemaphore();
	if (!CreatePipe(&rPipeAdd, &wPipeAdd, NULL, 0))
		return -1;
	if (!CreatePipe(&rPipeDiv, &wPipeDiv, NULL, 0))
		return -1;
	if (!CreatePipe(&rPipeMul, &wPipeMul, NULL, 0))
		return -1;
	if (!CreatePipe(&rPipeSub, &wPipeSub, NULL, 0))
		return -1;
	threadAdd = CreateThread(NULL, 0, &addThread, NULL, 0, NULL);
	threadDiv = CreateThread(NULL, 0, &divThread, NULL, 0, NULL);
	threadMul = CreateThread(NULL, 0, &mulThread, NULL, 0, NULL);
	threadSub = CreateThread(NULL, 0, &subThread, NULL, 0, NULL);
	#pragma endregion

	res = GetRes(wPipeAdd, rPipeAdd, semAdd, semAdd2, values[0], values[1]);
	Out(res);

	res = GetRes(wPipeAdd, rPipeAdd, semAdd, semAdd2, res, values[2]);
	Out(res);

	average = GetRes(wPipeDiv, rPipeDiv, semDiv, semDiv2, res, 3);
	Out(average);

	values[0] = GetRes(wPipeSub, rPipeSub, semSub, semSub2, values[0], average);
	Out(values[0]);

	values[1] = GetRes(wPipeSub, rPipeSub, semSub, semSub2, values[1], average);
	Out(values[1]);

	values[2] = GetRes(wPipeSub, rPipeSub, semSub, semSub2, values[2], average);
	Out(values[2]);

	values[0] = GetRes(wPipeMul, rPipeMul, semMul, semMul2, values[0], values[0]);
	Out(values[0]);

	values[1] = GetRes(wPipeMul, rPipeMul, semMul, semMul2, values[1], values[1]);
	Out(values[1]);

	values[2] = GetRes(wPipeMul, rPipeMul, semMul, semMul2, values[2], values[2]);
	Out(values[2]);

	res = GetRes(wPipeAdd, rPipeAdd, semAdd, semAdd2, values[0], values[1]);
	Out(res);

	res = GetRes(wPipeAdd, rPipeAdd, semAdd, semAdd2, res, values[2]);
	Out(res);

	res = GetRes(wPipeMul, rPipeMul, semMul, semMul2, res, 0.5);
	Out(res);

	std::cout << "Выборочная дисперсия = " << res << std::endl;

	////	1. Сложить три числа -> сумма
	//double sum = values[0] + values[1] + values[2];
	////	2. Поделить сумму на 3 -> cреднее
	//double average = sum / 3;
	////	3. Вычесть из каждого числа среднее -> 3 разности
	//values[0] -= average;
	//values[1] -= average;
	//values[2] -= average;
	////	4. Возвести каждую разность в квадрат -> 3 квадрата
	//values[0] *= values[0];
	//values[1] *= values[1];
	//values[2] *= values[2];
	////	5. Сложить три квадрата
	//sum = values[0] + values[1] + values[2];
	////	6. Умножить на 0.5
	//double dispersion = 0.5 * sum;
	//std::cout << "Выборочная дисперсия: " << dispersion << std::endl;
}