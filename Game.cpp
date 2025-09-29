#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <thread>
#include <chrono>
#include <random>
#include <deque>
#include <atomic>

#define SET_COLOR_TO_GRAY SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

using std::cout, std::endl, std::make_unique, std::thread, std::atomic, std::deque, std::unique_ptr;

constexpr int DINO_POS = 4;
constexpr int LAST_POS = 57;
constexpr auto JUMP_DURATION = std::chrono::milliseconds(570);
constexpr auto JUMP_DELAY = std::chrono::milliseconds(40);

struct Obstacle {
	unsigned short position, distance, passed, size;

	Obstacle() : position(LAST_POS), distance(7 + rand() % 15), passed(0), size(1 + rand() % 3) {}

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
deque<Obstacle> cactuses;

inline static void ChangeColorToGreen(HANDLE handle)
{
	const WORD color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(handle, color);
}

inline static void SetWindowSize(HWND hwnd, int width, int height) {
	if (hwnd) MoveWindow(hwnd, 450, 300, width, height, TRUE);
}

inline static void HideConsoleCursor(HANDLE handle)
{
	CONSOLE_CURSOR_INFO curs = { 0 };
	curs.dwSize = sizeof(curs);
	curs.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &curs);
}

inline static void CreateObstacle(std::deque<Obstacle> &cactuses) {
	cactuses.emplace_back();
}

inline void UpdateMap(Map &map, std::deque<Obstacle> &cactuses) {
	map.player[0] = ' ';

	for (int i = 0; i < cactuses.size(); i++) {
		int pos = cactuses[i].position;

		for (int j = 0; j < cactuses[i].size; j++) {
			if (pos - j >= 0 && pos - j <= LAST_POS)
				map.player[pos - j] = '#';
		}

		if (pos + 1 <= LAST_POS) map.player[pos + 1] = ' ';

		if (pos > 0) {
			cactuses[i].position--;
			cactuses[i].passed++;
		}
		else {
			cactuses.pop_front();
			--i;
		}
	}
}

void static Jump(Map *map)
{
	while (dinoAlive.load(std::memory_order_relaxed))
	{
		if (GetKeyState(VK_SPACE) < 0)
		{
			map->sky[DINO_POS] = 'D';
			map->player[DINO_POS] = ' ';

			std::this_thread::sleep_for(JUMP_DURATION);

			map->sky[DINO_POS] = ' ';
			map->player[DINO_POS] = 'D';

			std::this_thread::sleep_for(JUMP_DELAY);
		} else
			std::this_thread::sleep_for(JUMP_DELAY);
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

	SET_COLOR_TO_GRAY

	unsigned long long score = 0;

	CreateObstacle(cactuses);

	while (true)
	{
		SetWindowSize(hwnd, 500, 250);
		HideConsoleCursor(handle);
		SET_COLOR_TO_GRAY
		system("cls");
		
		thread jumpCheck(Jump, &map);

		while (dinoAlive.load(std::memory_order_relaxed))
		{
			if (map.player[4] == ' ' && map.sky[4] != 'D') {
				dinoAlive.store(false, std::memory_order_relaxed);
				jumpCheck.join();
			}

			if (cactuses.back().passed == cactuses.back().distance || cactuses.back().size == 0)
				CreateObstacle(cactuses);

			UpdateMap(map, cactuses);
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

		//////////////////////////////
		// Game Over screen handler //
		//////////////////////////////
		HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
		if (hInput == INVALID_HANDLE_VALUE) {
			std::cerr << "Ошибка: нет дескриптора ввода\n";
			return 1;
		}

		// включаем нужные режимы: клавиатура + стандартный ввод
		DWORD prevMode;
		GetConsoleMode(hInput, &prevMode);
		SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		cout << endl << "              GAME OVER";
		cout << endl << "              Your score: " << score << "   " << dinoAlive << endl;
		cout << endl << "   PRESS SPACE TO START AGAIN\n   OR ESC TO CLOSE THE GAME" << endl << endl;
		cout << lose_screen << endl;

		bool waiting = true;
		while (waiting) {
			INPUT_RECORD record;
			DWORD events;
			if (!ReadConsoleInput(hInput, &record, 1, &events))
				continue; // если ошибка чтения — просто ждём дальше

			if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
				switch (record.Event.KeyEvent.wVirtualKeyCode) {
				case VK_SPACE:
					dinoAlive.store(true, std::memory_order_relaxed);
					score = 0;
					for (int i = 0; i <= LAST_POS; ++i) map.player[i] = (i == 4 ? 'D' : ' ');
					cactuses.clear();
					CreateObstacle;
					waiting = false;
					break;
				case VK_ESCAPE:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					waiting = false;
					break;
				}
			}
		}

		SetConsoleMode(hInput, prevMode);
	}

	return 0;
}