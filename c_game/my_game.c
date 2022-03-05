#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <mmsystem.h>				// 멀티미디어 시스템
#pragma comment(lib, "winmm.lib")	// .wav파일만 가능	

#define WINDOW_MAX_X 99             // 콘솔 창의 최대 인덱스 위치 
#define WINDOW_MAX_Y 39 

#define GAME_MAX_X 65               // 게임화면의 최대 인덱스 위치 
#define GAME_MAX_Y WINDOW_MAX_Y

#define USER_X GAME_MAX_X/2         // User의 기본 시작 위치
#define USER_Y GAME_MAX_Y-1

#define MAX_BULLET 20               // 총알의 최대 개수
#define STAGE_TIME 60               // 스테이지 당 버티는 시간(초)
#define MIN_SCORE_TO_SKILL 200      // SKILL 하나 생성에 필요한 SCORE의 수

char * mon_face[] = { "§^＠^§", "§-○-§", "§*■*§" };
int GAME_OVER = 0, SCORE = 0, SKILL = 0, SKILL_TOT = 0;

typedef struct {
	int x;
	int y;
} User;

typedef struct {
	int x;
	int y;
	BOOL exist;
} Bullet;

typedef struct {
	int x;
	int y;
	int hit;   // 스킬의 총알 개수 
	BOOL exist;
} Skill;

typedef struct {
	int x;
	int y;
	int rannum;   // sinMove를 위한 랜덤 평행이동 값
	int pattern;  // 몬스터가 움직이는 패턴정보
	int life;     // 몬스터의 목숨개수
	BOOL exist;
} Monster;

User user = { 0 };
Bullet * bullet = NULL;
Skill skill[3] = { 0 };
Monster * monster = NULL;


enum {
	BLACK,
	D_BLUE,
	D_GREEN,
	D_SKYBLUE,
	D_RED,
	D_VIOLET,
	D_YELLOW,
	GRAY,
	D_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE,
};


void cursorHide();   // 커서 숨기기 함수
void gotoxy(int x, int y);   // 콘솔창의 위치 변경 함수
void setcolor(int color);  // 색상 변경 함수



void userPrint(int x, int y);
void monsterPrint(int i);
void bulletPrint(int i);
void skillPrint(i);


void userErase();
void monsterErase(int i);
void bulletErase(int i);
void skillErase(int i);


void monsterDelete(int i);
void bulletDelete(int i);
void skillDelete(int i);




void stageView(int stage);  // 스테이지 안내 화면

void end();                 // 종료 화면
void game_over();           // 게임 오버 화면
void game_clear();          // 게임 클리어 화면

void gameView();            // 게임 화면 오른쪽 진행판

void timeView(int stage_start_time); // 남은 시간 출력

void scoreView();           // 현재 점수 출력
void skillView();           //  스킬 개수 출력

void scoreGet(int i);       // 몬스터 pattern에 따라 얻는 SCORE 정보 갱신
void skillGet();            // SKILL 정보 갱신





int monsterMax(int stage);     // 스테이지 당 화면에 나타나는 최대 몬스터 수
int monsterSpeed(int stage);   // 스테이지 당 몬스터의 생성, 움직임 속도
int monsterPattern(int stage); // 스테이지 당 몬스터 내려오는 패턴







int monsterGen(int max_monster, int stage);  // 몬스터 한 마리를 생성하고 출력
void downMove(int i);                        // 몬스터를 아래로 한 칸 이동하고 출력
void sinMove(int i);                         // 몬스터를 sin함수 이용해서 한 칸 이동하고 출력

void monsterGenMove(int max_monster, int stage);  
// 몬스터 생성하고, 전체를 지우고, (생성한 몬스터에 인덱스빼고) 패턴에 따라 한 칸 이동 



void userKeyInput(int max_bullet);

void bulletMove(int max_bullet);  // 총알 전체 이동
void skillMove();                 // 스킬 전체 이동

void crash(int max_monster, int max_bullet);   // 총알과 몬스터 충돌 시 판정





void game() {

	PlaySound(TEXT("../music/game.wav"), NULL, SND_ASYNC | SND_LOOP);

	srand(time(NULL));

	int stage = 0, max_monster, max_bullet, \
		stage_start_time = 0, prev_gen_time = 0;

	while (1) {

		system("cls");

		stage++;
		if (stage == 4) {
			game_clear();
			break;
		}
		
		max_monster = monsterMax(stage);
		max_bullet = MAX_BULLET;

		if (monster != NULL) {
			free(monster);
		}
		if (bullet != NULL) {
			free(bullet);
		}

		monster = (Monster *)calloc(max_monster, sizeof(Monster));
		bullet = (Bullet *)calloc(max_bullet, sizeof(Bullet));

		user.x = USER_X-4;
		user.y = USER_Y;

		stageView(stage);

		gameView();
		userPrint(user.x, user.y); 
		scoreView();
		skillGet();
		skillView();

		stage_start_time = time(NULL);

		if(_kbhit())
			while (_kbhit()) _getch();    // 스테이지 화면에서 발생하는 키입력의 버퍼를 비워준다. 

		for (int i = 0; i < 3; i++) {     // 화면에 남아있는 스킬을 지워준다.
			skillDelete(i);
		}


		while(time(NULL) - stage_start_time < STAGE_TIME) { // STAGE_TIME 이상 넘어가면 다음 스테이지로.

			timeView(stage_start_time);

			userKeyInput(max_bullet);
			crash(max_monster, max_bullet);

			bulletMove(max_bullet);
			skillMove();
			crash(max_monster, max_bullet);

			if (clock() - prev_gen_time >= monsterSpeed(stage)) { // 정해진 시간 마다 몬스터 생성 & 이동.
				prev_gen_time = clock();

				monsterGenMove(max_monster, stage);

				if (GAME_OVER == 1) {
					break;
				}
			}
			crash(max_monster, max_bullet);

			Sleep(40);
		}

		if (GAME_OVER == 1) {
			game_over();
			break;
		}
	}
}




void menu() {   // 메뉴 선택 화면

	system("cls");

	PlaySound(TEXT("../music/game.wav"), NULL, SND_ASYNC | SND_LOOP);

	setcolor(WHITE);
	gotoxy(WINDOW_MAX_X / 2 - 15, 10);
	printf("S H O O T I N G  S H O O T I N G");

	userPrint(WINDOW_MAX_X / 2, 17);

	gotoxy(WINDOW_MAX_X / 2 - 8, 25);
	printf("┌───────────────┐ ");
	gotoxy(WINDOW_MAX_X / 2 - 8, 26);
	printf("│               │ ");
	gotoxy(WINDOW_MAX_X / 2 - 8, 27);
	printf("│     게임시작  │ ");
	gotoxy(WINDOW_MAX_X / 2 - 8, 28);
	printf("│     종료      │ ");
	gotoxy(WINDOW_MAX_X / 2 - 8, 29);
	printf("│               │ ");
	gotoxy(WINDOW_MAX_X / 2 - 8, 30);
	printf("└───────────────┘ ");

	int select = 1;   // 화살표의 현재 위치 1 = 위, 0 = 아래
	gotoxy(WINDOW_MAX_X / 2 - 5, 27);
	printf("▶");
	while (1)
	{
		if (GetAsyncKeyState(VK_DOWN) && (select == 1)) {
			Beep(320, 200);
			gotoxy(WINDOW_MAX_X / 2 - 5, 28);
			printf("▶");
			gotoxy(WINDOW_MAX_X / 2 - 5, 27);
			printf("  ");
			select = 0;
		}
		else if (GetAsyncKeyState(VK_UP) && (select == 0)) {
			Beep(320, 200);
			gotoxy(WINDOW_MAX_X / 2 - 5, 27);
			printf("▶");
			gotoxy(WINDOW_MAX_X / 2 - 5, 28);
			printf("  ");
			select = 1;
		}
		if (GetAsyncKeyState(VK_RETURN) && (select == 1)) {
			Beep(410, 200);
			game();
			break;
		}
		else if (GetAsyncKeyState(VK_RETURN) && (select == 0)) {
			Beep(410, 200);
			end();
			break;
		}
	}
}

int main() {

	cursorHide();
	system("mode CON COLS=100 LINES=40");   // 콘솔창 크기 조절 cols = 열(x), lines = 행(y)
	menu();

	if (monster != NULL) {    // 남아있는 calloc함수의 메모리 할당 영역을 비워주기 위해서 
		free(monster);
	}
	if (bullet != NULL) {
		free(bullet);
	}
	return 0;
}












void cursorHide() {    // 커서 숨기기 함수
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1; // 커서 굵기 (1 ~ 100)
	cursorInfo.bVisible = FALSE; // 커서 Visible TRUE(보임) FALSE(숨김)
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y) {    // 콘솔창의 위치 변경 함수
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}


void setcolor(int color)  // 색상 변경 함수
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}





void userPrint(int x, int y) {
	setcolor(WHITE);
	gotoxy(x, y);
	printf("▲");
	gotoxy(x - 4, y + 1);
	printf("△◎□◎△");
}

void monsterPrint(int i) {   // 8칸
	if (monster[i].exist == 1) {
		gotoxy(monster[i].x, monster[i].y);
		switch (monster[i].life) {
		case 3:
			setcolor(GREEN);
			printf("%s", mon_face[0]);
			break;
		case 2:
			setcolor(YELLOW);
			printf("%s", mon_face[1]);
			break;
		case 1:
			setcolor(RED);
			printf("%s", mon_face[2]);
			break;
		}
	}
}

void bulletPrint(int i) {
	setcolor(WHITE);
	if (bullet[i].exist == TRUE) {
		gotoxy(bullet[i].x, bullet[i].y);
		printf("*");
	}
}

void skillPrint(i) {
	setcolor(WHITE);
	if (skill[i].exist == TRUE) {
		switch (i) {
		case 0:
			gotoxy(skill[0].x, skill[0].y);
			printf("*");
			break;
		case 1:
			gotoxy(skill[1].x, skill[1].y);
			printf("***");
			break;
		case 2:
			gotoxy(skill[2].x, skill[2].y);
			printf("*****");
			break;
		}
	}
}






void userErase() {
	gotoxy(user.x, user.y);
	printf("  ");
	gotoxy(user.x - 4, user.y + 1);
	printf("           ");
}

void monsterErase(int i) {
	gotoxy(monster[i].x, monster[i].y);
	printf("        ");
}

void bulletErase(int i) {
	gotoxy(bullet[i].x, bullet[i].y);
	printf(" ");
}

void skillErase(int i) {
	switch (i) {
	case 0:
		gotoxy(skill[i].x, skill[i].y);
		printf(" ");
	case 1:
		gotoxy(skill[i].x, skill[i].y);
		printf("   ");
	case 2:
		gotoxy(skill[i].x, skill[i].y);
		printf("     ");
	}
}

void monsterDelete(int i) {
	monsterErase(i);
	monster[i].x = 0;
	monster[i].y = 0;
	monster[i].rannum = 0;
	monster[i].life = 0;
	monster[i].exist = FALSE;
}

void bulletDelete(int i) {
	bulletErase(i);
	bullet[i].x = 0;
	bullet[i].y = 0;
	bullet[i].exist = FALSE;
}

void skillDelete(int i) {
	skillErase(i);
	skill[i].x = 0;
	skill[i].y = 0;
	skill[i].hit = 0;
	skill[i].exist = FALSE;
}









void stageView(int stage) {  // 스테이지 안내 화면
	system("cls");
	setcolor(WHITE);
	gotoxy(WINDOW_MAX_X / 2 - 3, WINDOW_MAX_Y / 2);
	printf("Stage %d.", stage);
	Sleep(2000);
	system("cls");
}

void end() {    // 종료 화면
	PlaySound(TEXT("../music/end.wav"), NULL, SND_ASYNC | SND_LOOP);
	system("cls");
	setcolor(WHITE);
	gotoxy(WINDOW_MAX_X / 2 - 3, WINDOW_MAX_Y / 2);
	printf("게임 종료.");
	gotoxy(WINDOW_MAX_X, WINDOW_MAX_Y);

	Sleep(1000);
}

void game_over() {   // 게임 오버 화면
	PlaySound(TEXT("../music/gameover.wav"), NULL, SND_ASYNC | SND_LOOP);
	system("cls");
	setcolor(WHITE);
	gotoxy(WINDOW_MAX_X / 2 - 3, WINDOW_MAX_Y / 2);
	printf("게임 오버.");
	gotoxy(WINDOW_MAX_X, WINDOW_MAX_Y);

	Sleep(1300);
}

void game_clear() {    // 게임 클리어 화면
	PlaySound(TEXT("../music/finish.wav"), NULL, SND_ASYNC | SND_LOOP);
	system("cls");
	setcolor(WHITE);
	gotoxy(WINDOW_MAX_X / 2 - 4, WINDOW_MAX_Y / 2 - 1);
	printf("게임 클리어.");
	gotoxy(WINDOW_MAX_X / 2 - 12, WINDOW_MAX_Y / 2);
	printf("당신의 점수는 %4d점 입니다.", SCORE);
	gotoxy(WINDOW_MAX_X, WINDOW_MAX_Y);

	Sleep(5000);
}

void gameView() {    // 게임 화면 오른쪽 진행판
	setcolor(WHITE);
	for (int i = 0; i <= WINDOW_MAX_Y; i++) {
		gotoxy(GAME_MAX_X + 1, i);
		printf("┃");
	}
	gotoxy(GAME_MAX_X + 9, 8);
	printf("%d초 동안 버티세요!", STAGE_TIME);
	gotoxy(GAME_MAX_X + 8, 14);
	printf("남은 시간 : ");
	gotoxy(GAME_MAX_X + 8, 16);
	printf("점수 : ");
	gotoxy(GAME_MAX_X + 8, 18);
	printf("남은 스킬 : ");
	gotoxy(GAME_MAX_X + 8, 24);
	printf("왼쪽 이동 : ← ");
	gotoxy(GAME_MAX_X + 8, 26);
	printf("오른쪽 이동 : →");
	gotoxy(GAME_MAX_X + 8, 28);
	printf("총알 쏘기 : 스페이스 바");
	gotoxy(GAME_MAX_X + 8, 30);
	printf("스킬 쏘기 : 엔터키");

}

void timeView(int stage_start_time) {  // 남은 시간 출력 
	setcolor(WHITE);
	int now = time(NULL);
	int time = (STAGE_TIME - (now - stage_start_time));  // 스테이지 시작 ~ 현재의 시간 = time
	gotoxy(GAME_MAX_X + 22, 14);
	printf("%2d초", time);
}

void scoreView() {    // 현재 점수 출력
	setcolor(WHITE);
	gotoxy(GAME_MAX_X + 17, 16);
	printf("%4d점", SCORE);
}

void skillView() {    //  스킬 개수 출력
	setcolor(WHITE);
	gotoxy(GAME_MAX_X + 22, 18);
	printf("%2d개", SKILL);
}

void scoreGet(int i) {   // 몬스터 pattern에 따라 얻는 SCORE 정보 갱신
	if (monster[i].life == 0) {
		SCORE += (10 * monster[i].pattern);   // kill : pattern 1 = 10, pattern 2 = 20
	}
	else {
		SCORE += (3 * monster[i].pattern);    // hit : pattern 1 = 3, pattern 2 = 6
	}
}

void skillGet() {    // SKILL 정보 갱신
	int sk;
	sk = ((SCORE - (SKILL_TOT * MIN_SCORE_TO_SKILL)) / MIN_SCORE_TO_SKILL);
	// 이미 얻은 점수에 대해서는 제외하고 새로 얻은 점수로만 skill의 개수를 계산함. 이번에 얻은 SKILL = sk
	SKILL_TOT += sk;    // SKILL_TOT은 현재까지 얻었던 총 skill의 개수
	SKILL += sk;
}








int monsterMax(int stage) {  // 스테이지 당 화면에 나타나는 몬스터 최대 수 
	switch (stage) {
	case 1:
		return 10;
		break;
	case 2:
		return 15;
		break;
	case 3:
		return 20;
		break;
	}
}

int monsterSpeed(int stage) {  // 스테이지 당 몬스터의 움직임 속도
	switch (stage) {
	case 1:
		return 600;   // 1000밀리초 = 1초
		break;
	case 2:
		return 500;
		break;
	case 3:
		return 400;
		break;
	}
}

int monsterPattern(int stage) {  // 스테이지 당 몬스터 내려오는 패턴
	int random = rand(time(NULL)) % 2 + 1; // random pattern number = 1 or 2

	switch (stage) {
	case 1:
		return 1;
		break;
	case 2:
		return 2;
		break;
	case 3:
		return random;
		break;
	}
}








int monsterGen(int max_monster, int stage) {  // 몬스터 한 마리를 생성하고 출력함
	int i, rx, rpar, rpat;

	for (i = 0; i < max_monster; i++) {
		if (monster[i].exist == FALSE) {

			rx = (rand() % (GAME_MAX_X - 8));  // random x location number = 0~57
			monster[i].x = rx;
			monster[i].y = 0;
			monster[i].life = 3;
			monster[i].exist = TRUE;

			rpar = rand(time(NULL)) % 7;   // random parallel translation number for sin = 0~6 (0~2*PI)
			monster[i].rannum = rpar;

			rpat = monsterPattern(stage);   // pick random pattern number = 1 or 2
			monster[i].pattern = rpat;

			monsterPrint(i);
			return i;

		}
	}
}

void downMove(int i) {   // 몬스터를 아래로 한 칸 이동, 출력

	monster[i].y++;
	monsterPrint(i);
}

void sinMove(int i) {     // 몬스터를 sin함수 이용해서 한 칸 이동, 출력
	double si, degree;

	monster[i].y++;
	degree = monster[i].y * 9;
	// y값이 0부터 39까지 증가됨에 따라 degree가 0부터 360이 된다.
	// y값 1칸은 9도
	si = sin(degree*3.14 / 180.0 + monster[i].rannum);
	// 해당 degree의 radian값에 몬스터의 고유 random값만큼 평행이동
	monster[i].x = si * 27 + 30;
	// 폭(가로x축): 27, 함수와의 거리: 30
	monsterPrint(i);

}

void monsterGenMove(int max_monster, int stage) {  // 몬스터 Gen하고, 전체를 지우고, (Gen한 idx빼고) 패턴에 따라 한 칸 이동 
	int i;
	int idx = monsterGen(max_monster, stage);

	for (i = 0; i < max_monster; i++) {
		if (monster[i].exist == TRUE) {
			monsterErase(i);
		}
	}

	monsterPrint(idx);

	for (i = 0; i < max_monster; i++) {
		if (i == idx || monster[i].exist == FALSE) {
			continue;
		}
		else {
			if (monster[i].y == WINDOW_MAX_Y) {  // 화면을 넘어서면 종료
				monster[i].exist = FALSE;
				GAME_OVER = 1;
				return;
			}
			else {                             // 화면 안에 있다면 pattern값에 따라 움직인다.
				switch (monster[i].pattern) {
				case 1:
					downMove(i);
					break;
				case 2:
					sinMove(i);
					break;
				}
			}
		}
	}

}










void userKeyInput(int max_bullet) {
	// GetAsyncKeyState = 눌려진 상태(한 번 눌렀지만 여러번 입력됨), _kbhit = 눌렀다 띄는 순간(한번 누르면 한 번 입력)

	if (GetAsyncKeyState(VK_LEFT) && (user.x > 4)) {  // 유저 키조작: 좌, 우 
		userErase();
		user.x--;
		userPrint(user.x, user.y);
	}
	else if (GetAsyncKeyState(VK_RIGHT) && (user.x < GAME_MAX_X - 8)) {
		userErase();
		user.x++;
		userPrint(user.x, user.y);
	}



	char ch;
	int i, cnt = 1;

	if (_kbhit()) {
		ch = _getch();
		switch (ch) {
		case VK_SPACE:     // 유저 키조작: SPACE BAR
			for (i = 0; i < max_bullet; i++) {
				if (bullet[i].exist == FALSE) {
					bullet[i].x = user.x + 1;   // 총알을 화면 상 사용자 위치의 가운데에 맞추기 위해 +1
					bullet[i].y = WINDOW_MAX_Y - 2;
					bullet[i].exist = TRUE;

					bulletPrint(i);
					return;
				}
			}
			break;
		case VK_RETURN:    // 유저 키조작: ENTER KEY
			if (skill[0].exist == FALSE && skill[1].exist == FALSE && skill[2].exist == FALSE && SKILL > 0) {
				// 스킬은 한 번에 하나만 사용 가능
				for (i = 0; i < 3; i++) {
					skill[i].x = user.x - i + 1;
					skill[i].y = WINDOW_MAX_Y - 4 + i;
					skill[i].hit = i + cnt; // 1  3  5
					cnt++;
					skill[i].exist = TRUE;
					skillPrint(i);
				}
				SKILL--;
				skillGet();
				skillView();
				return;
			}
			break;
		}
	}

}

void bulletMove(int max_bullet) {   // 총알 전체 이동
	int i;
	for (i = 0; i < max_bullet; i++) {
		if (bullet[i].exist == TRUE) {
			bulletErase(i);
		}
	}
	for (i = 0; i < max_bullet; i++) {
		if (bullet[i].exist == TRUE) {
			if (bullet[i].y == 0) {
				bulletDelete(i);
			}
			else {
				bullet[i].y--;
				bulletPrint(i);
			}
		}
	}
}

void skillMove() {   // 스킬 총알 전체 이동
	int i;

	for (i = 0; i < 3; i++) {
		if (skill[i].exist == TRUE) {
			skillErase(i);
			if (skill[i].y == 0) {
				skillDelete(i);
			}
			else if (skill[i].y > 0) {
				skill[i].y--;
				skillPrint(i);
			}
		}
	}
}



void crash(int max_monster, int max_bullet) {   // 총알과 몬스터 충돌 시
	int i, j, k, s;

	for (i = 0; i < max_monster; i++) {
		if (monster[i].exist == FALSE) {
			continue;
		}

		for (j = 0; j < max_bullet; j++) {
			if (bullet[j].exist == FALSE) {
				continue;
			}
			// 몬스터가 있다면, 총알이 있다면, 몬스터와 총알이 만났다면.
			if ((abs(bullet[j].y - monster[i].y) <= 1) && (abs((bullet[j].x - 3) - monster[i].x) < 5)) {
				// 총알 실제 위치가 보이는 위치값x 보다 3칸 오른쪽에 존재하기 때문에 (bullet[j].x-3)

				bulletDelete(j);
				monster[i].life--;

				if (monster[i].life <= 0) {
					monsterDelete(i);
				}
				else {
					monsterPrint(i);
				}
				scoreGet(i);
			}
		}


		for (k = 0; k < 3; k++) {
			if (skill[k].exist == FALSE) {
				continue;
			}
			// 몬스터가 있다면, 스킬이 있다면, 몬스터와 스킬이 만났다면.
			if ((abs(skill[k].y - monster[i].y) <= 1) && (abs((skill[k].x - 3) - monster[i].x) < 5 + k * 2)) {

				if (monster[i].life <= skill[k].hit) {
					// 몬스터와 스킬 모두 사라지는 상황 ( 2:3 or 2:2 ) 스킬은 한번 부딪히면 한줄이 사라진다.
					skillDelete(k);
					for (s = 0; s < monster[i].life - 1; s++) {
						monster[i].life--;
						monsterPrint(i);
					}
					monsterDelete(i);
				}
				else {          // 몬스터가 살아남는 상황
					skillDelete(k);
					monster[i].life--;
					monsterPrint(i);
				}
				scoreGet(k);
			}
		}
		scoreView();
		skillGet();
		skillView();
	}
}


