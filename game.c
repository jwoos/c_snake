#include "game.h"


void ncursesSetup(void) {
	// set locale to support wide character
	setlocale(LC_CTYPE, "");

	// start ncurses
	initscr();
	clear();

	/* pass EVERYTHING as keycodes, even ignoring signals
	 * disables buffering and prints directly
	 */
	raw();

	// Take non alphanumeric keys
	keypad(stdscr, TRUE);

	// don't echo while getting input
	noecho();

	// no delay
	nodelay(stdscr, TRUE);

	// don't show cursor
	curs_set(FALSE);
}

void ncursesTeardown(void) {
	// clear to end of line
	clrtoeol();

	// clean up ncurses
	endwin();
}

void gamePreSetup(void) {
	ncursesSetup();
}

void gameSetup(void) {
	// seed rng
	srand(time(NULL));

	Config.minX = 0;
	Config.minY = 0;
	getmaxyx(stdscr, Config.maxY, Config.maxX);

	Config.itemTimer = timerRegister(SIGRTMIN, 3e9, itemTimerHandler);

	// reference as [y][x]
	Config.board = itemConstruct();

	Config.timespec.tv_sec = TIMESPEC_SEC;
	Config.timespec.tv_nsec = TIMESPEC_NANOSEC;

	box(stdscr, 0, 0);
	Config.items = vectorConstruct(16);
	Config.snake = snakeConstruct();

	Config.sigCount = 0;
}

void gamePostSetup(void) {

}

void gamePreTeardown(void) {
	timerDeregister(Config.itemTimer);
	if (Config.debug) {
		gameEndScreenDebug();
	} else {
		gameEndScreen();
	}
}

void gameTeardown(void) {
	itemDeconstruct(Config.board);
	vectorDeconstruct(Config.items, positionDeconstruct);
	snakeDeconstruct(Config.snake);
}

void gamePostTeardown(void) {
	ncursesTeardown();
}

DirectionOrientation parseInput(int ch) {
	DirectionOrientation direction;

	switch (ch) {
		case KEY_UP:
			direction = DIRECTION_ORIENTATION_UP;
			break;

		case KEY_DOWN:
			direction = DIRECTION_ORIENTATION_DOWN;
			break;

		case KEY_RIGHT:
			direction = DIRECTION_ORIENTATION_RIGHT;
			break;

		case KEY_LEFT:
			direction = DIRECTION_ORIENTATION_LEFT;
			break;

		case 'a':
			if (Config.debug) {
				snakeAdd(Config.snake);
			}
			direction = directionGet(Config.snake -> direction);
			break;

		case 's':
			if (Config.debug) {
				itemAdd(Config.board, Config.items, Config.snake);
			}
			direction = directionGet(Config.snake -> direction);
			break;

		// quit
		case 'q':
			direction = DIRECTION_ORIENTATION_NONE;
			break;

		default:
			direction = directionGet(Config.snake -> direction);
			break;
	}

	return direction;
}

void gameEndScreen(void) {
	nodelay(stdscr, FALSE);
	refresh();
	mvprintw(1, 1, "Total items consumed: %d", Config.snake -> body -> size - 1);
	mvprintw(2, 1, "Press any key to quit");
	getch();
}

void gameEndScreenDebug(void) {
	Position* position = vectorGet(Config.snake -> body, 0);
	nodelay(stdscr, FALSE);
	refresh();
	mvprintw(1, 1, "x: %d y: %d", position -> x, position -> y);
	mvprintw(2, 1, "max x: %d max y: %d", Config.maxX, Config.maxY);
	mvprintw(3, 1, "snake length: %d", Config.snake -> body -> size);

	uint64_t i;
	for (i = 0; i < Config.snake -> body -> size; i++) {
		position = vectorGet(Config.snake -> body, i);
		mvprintw(4 + i, 1, "snake position %d: %d %d", i, position -> x, position -> y);
	}

	mvprintw(4 + i, 1, "items count: %d", Config.items -> size);

	uint64_t j;
	// segfaulting
	for (j = 0; j < Config.items -> size; j++) {
		position = vectorGet(Config.items, j);
		mvprintw(5 + j + i, 1, "item position %d: %d %d", j, position -> x, position -> y);
	}
	mvprintw(i + j + 5, 1, "RT Signals handled: %d", Config.sigCount);
	mvprintw(i + j + 6, 1, "Press any key to quit");
	getch();
}
