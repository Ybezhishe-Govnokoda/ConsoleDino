#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <thread>
#include <chrono>
#include <random>
#include <deque>
#include <atomic>

using std::cout, std::endl, std::thread, std::atomic, std::deque;

struct Obstacle {
	unsigned short position = 57, distance, passed = 0, size = 0;

	Obstacle() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr_distance(7, 15), distr_size(1, 3);

		size = distr_size(gen);
		distance = distr_distance(gen);
	}

	bool distance_check() const
	{
		if (size == 0) return true;
		if (distance == passed) return true;
		return false;
	}
};

struct Map {
	char ground[59] = "[][][][][][][][][][][][][][][][][][][][][][][][][][][][][]";
	char player[59] = "    D                                                     ";
	char sky[6] = "     ";
};

static atomic<bool> dinoAlive{ true };
deque<Obstacle *> cactuses;

constexpr int LAST_POS = 57;

#define ChangeColorToGreen(handle) \
	do{\
		const WORD color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;\
		SetConsoleTextAttribute(handle, color);\
	} while(0)

#define SetWindowSize(hwnd, width, height) \
	do {\
		if (hwnd != NULL)\
			MoveWindow(hwnd, 450, 300, width, height, TRUE);\
	} while(0)

#define HideConsoleCursor(handle) \
	do {\
		CONSOLE_CURSOR_INFO curs = { 0 };\
		curs.dwSize = sizeof(curs);\
		curs.bVisible = FALSE;\
		SetConsoleCursorInfo(handle, &curs);\
	} while(0)

#define CreateObstacle\
	do {\
		Obstacle *newbie = new Obstacle;\
		cactuses.push_back(newbie);\
	} while(0)

#define UpdateMap \
	do {\
		map.player[0] = ' ';\
		for (int i = 0; i < cactuses.size(); i++)\
		{\
			int p = cactuses[i]->position;\
			switch (cactuses[i]->size)\
			{\
				case 1:\
					if (p >= 0 && p <= LAST_POS) map.player[p] = '#';\
					break;\
				case 2:\
					if (p >= 0 && p <= LAST_POS) map.player[p] = '#';\
					if (p - 1 >= 0 && p - 1 <= LAST_POS) map.player[p - 1] = '#';\
					break;\
				case 3:\
					if (p >= 0 && p <= LAST_POS) map.player[p] = '#';\
					if (p - 1 >= 0 && p - 1 <= LAST_POS) map.player[p - 1] = '#';\
					if (p - 2 >= 0 && p - 2 <= LAST_POS) map.player[p - 2] = '#';\
					break;\
			}\
			if (p + 1 <= LAST_POS) map.player[p + 1] = ' ';\
			if (p > 0) {\
				cactuses[i]->position--;\
				cactuses[i]->passed++;\
			} else {\
				cactuses.pop_front();\
				--i;\
			}\
		}\
	} while(0)

void Jump(Map *map)
{
	while (dinoAlive.load(std::memory_order_relaxed))
	{
		if (GetKeyState(VK_SPACE) < 0)
		{
			map->sky[4] = 'D';
			map->player[4] = ' ';

			std::this_thread::sleep_for(std::chrono::milliseconds(570));

			map->sky[4] = ' ';
			map->player[4] = 'D';

			std::this_thread::sleep_for(std::chrono::milliseconds(80));
		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

int main() {
	HWND hwnd = GetConsoleWindow();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetWindowSize(hwnd, 665, 250);

	Map map;

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

	ChangeColorToGreen(handle);
	HideConsoleCursor(handle);

	cout << logo << endl << endl;
	cout << "	PRESS SPACE TO START";

	while (true) if (GetKeyState(VK_SPACE) < 0) break;
	system("cls");

	// Set color back to gray
	SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	unsigned long long score = 0;

	CreateObstacle;

	while (true)
	{
		SetWindowSize(hwnd, 500, 250);
		HideConsoleCursor(handle);
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		system("cls");
		
		thread jumpCheck(Jump, &map);
		jumpCheck.detach();

		while (dinoAlive.load(std::memory_order_relaxed))
		{
			if (map.player[4] == ' ' && map.sky[4] != 'D')
				dinoAlive.store(false, std::memory_order_relaxed);

			if (cactuses[cactuses.size() - 1]->distance_check()) CreateObstacle;

			UpdateMap;
			SetConsoleCursorPosition(handle, { 0, 0 });

			cout << endl << endl << endl << endl << map.sky << endl;
			cout << map.player;
			cout << map.ground << endl;
			cout << endl << "Score: " << score << endl;

			score++;
			Sleep(100);
		}

		system("cls");

		SetWindowSize(hwnd, 500, 350);
		HideConsoleCursor(handle);
		ChangeColorToGreen(handle);

		cout << endl << "              GAME OVER";
		cout << endl << "              Your score: " << score << "   " << dinoAlive << endl;
		cout << endl << "   PRESS SPACE TO START AGAIN\n   OR ESC TO CLOSE THE GAME" << endl << endl;
		cout << lose_screen << endl;

		while (true)
		{
			if (GetKeyState(VK_SPACE) < 0) {
				dinoAlive.store(true, std::memory_order_relaxed);
				score = 0;
				for (int i = 0; i <= LAST_POS; ++i) map.player[i] = (i == 4 ? 'D' : ' ');
				cactuses.clear();
				CreateObstacle;
				break;
			}
			else if (GetKeyState(VK_ESCAPE) < 0) {
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				break;
			}
		}
	}

	return 0;
}