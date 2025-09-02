#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <thread>
#include <chrono>
#include <random>

#include "Obstacle.h"
#include "List.h"

using std::cout;
using std::endl;
using std::thread;

char ground[] = "[][][][][][][][][][][][][][][][][][][][][][][][][][][][][]";
char player[] = "    D                                                     ";
char sky[] = "                                                          ";

#include <atomic>
static std::atomic<bool> dinoAlive{ true };

List<Obstacle *> cactuses;

constexpr int LAST_POS = 57;

#define ChangeColorToGreen \
	do{\
		const WORD color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;\
		SetConsoleTextAttribute(handle, color);\
	} while(0)

#define SetWindowSize(width, height) \
	do {\
		if (hwnd != NULL)\
			MoveWindow(hwnd, 450, 300, width, height, TRUE);\
	} while(0)

#define HideConsoleCursor \
	do {\
		CONSOLE_CURSOR_INFO curs = { 0 };\
		curs.dwSize = sizeof(curs);\
		curs.bVisible = FALSE;\
		::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &curs);\
	} while(0)

#define CreateObstacle\
	do {\
		Obstacle *newbie = new Obstacle;\
		cactuses.push_back(newbie);\
	} while(0)

#define UpdateMap \
	do {\
		player[0] = ' ';\
		for (int i = 0; i < cactuses.get_size(); i++)\
		{\
			int p = cactuses[i]->get_pos();\
			if (p >= 0 && p <= LAST_POS) player[p] = '#';\
			if (p + 1 <= LAST_POS) player[p + 1] = ' ';\
			if (p > 0) {\
				cactuses[i]->pos_decrement();\
				cactuses[i]->passed_increment();\
			} else {\
				cactuses.pop_front();\
				--i;\
			}\
		}\
	} while(0)

void Jump()
{
	while (dinoAlive.load(std::memory_order_relaxed))
	{
		if (GetKeyState(VK_SPACE) < 0)
		{
			sky[4] = 'D';
			player[4] = ' ';

			Sleep(550);

			sky[4] = ' ';
			player[4] = 'D';

			std::this_thread::sleep_for(std::chrono::milliseconds(80));
		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

int main() {
	HWND hwnd = GetConsoleWindow();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { 0, 0 };

	SetWindowSize(665, 250);
	HideConsoleCursor;

	const char *logo = R"(

	  ____ ___  _   _ ____   ___  _     _____      ____ ___ _   _  ___  
	 / ___/ _ \| \ | / ___| / _ \| |   | ____|    |  _ \_ _| \ | |/ _ \ 
	| |  | | | |  \| \___ \| | | | |   |  _|      | | | | ||  \| | | | |
	| |__| |_| | |\  |___) | |_| | |___| |___     | |_| | || |\  | |_| |
	 \____\___/|_| \_|____/ \___/|_____|_____|    |____/___|_| \_|\___/ 

	)";

	const char *lose_screen = R"(

	__   __            _                       __
	\ \ / /__  _   _  | | ___  ___  ___   _   / /
	 \ V / _ \| | | | | |/ _ \/ __|/ _ \ (_) | | 
	  | | (_) | |_| | | | (_) \__ \  __/  _  | | 
	  |_|\___/ \__,_| |_|\___/|___/\___| (_) | | 
	                                          \_\
	)";

	ChangeColorToGreen;

	cout << logo << endl << endl;
	cout << "	PRESS SPACE TO START";

	while (true) if (GetKeyState(VK_SPACE) < 0) break;
	system("cls");

	// Set color back to gray
	SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	unsigned long long score = 0;

	thread jumpCheck(Jump);
	jumpCheck.detach();

	CreateObstacle;

	while (true)
	{
		SetWindowSize(500, 250);

		while (dinoAlive.load(std::memory_order_relaxed))
		{
			if (player[4] == ' ' && sky[4] != 'D')
				dinoAlive.store(false, std::memory_order_relaxed);

			if (cactuses[cactuses.get_size() - 1]->distance_check()) CreateObstacle;

			UpdateMap;
			SetConsoleCursorPosition(handle, pos);

			cout << endl << endl << endl << endl << sky;
			cout << player;
			cout << ground << endl;
			cout << endl << "Score: " << score << endl;

			score++;
			Sleep(100);
		}

		system("cls");

		SetWindowSize(500, 350);
		HideConsoleCursor;
		ChangeColorToGreen;

		cout << endl << "              GAME OVER";
		cout << endl << "              Your score: " << score << endl;
		cout << endl << "   PRESS SPACE TO START AGAIN\n   OR ESC TO CLOSE THE GAME" << endl << endl;
		cout << lose_screen << endl;

		while (true)
		{
			if (GetKeyState(VK_SPACE) < 0) {
				dinoAlive.store(false, std::memory_order_relaxed);
				break;
			}
			else if (GetKeyState(VK_ESCAPE) < 0) PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
	}

	return 0;
}