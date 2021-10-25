/*------------------------------------------------*/
/*gcc -o game game.c `pkg-config --libs ncurses`*/
/* 51ncurses/terminal.c                           */
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#define X 60
#define Y 30
#define C_LEN 8

#define C_GRASS 1
#define C_AVATAR 2
#define C_WATER 3

#define C_HERO COLOR_RED

int mapa[Y][X] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,0,0,0},
	{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0},
	{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0},
	{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0},
	{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,2,2,2,2,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

typedef struct{
	int x;
	int y;
}coords;

typedef struct{
	int bg;
	int fg;
	int perm;
	const char * symbol;
}pole;

typedef enum{
	knife = 0, sniper, shotgun, bomb
}role;

typedef enum{
	left = 0, right, up, down
}direction;

typedef struct{
	int fg;
	role rol;
	int live;
	const char * symbol;
	coords cd;
}hrac;

pole pole_list[20];

hrac hrac_list[5];

static void obnovTerminal()
{
    printw("Obnovuji nastavení terminálu\n");
    endwin();
}

void pairInitialization(void){
	for (int i = 0; i < C_LEN; i++) {
        for (int j = 0; j < C_LEN; j++) {
            init_pair(i*10+j, i, j);
        }
    }
}

void vypis_mapu(void){
	int type=0;
	for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
			type=mapa[i][j];
            attrset(COLOR_PAIR(pole_list[type].fg*10+pole_list[type].bg));
            mvprintw(i, j, pole_list[type].symbol);
        }
    }
}

/*
void uklid(void){
	for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
            attrset(COLOR_PAIR(COLOR_GREEN*10+COLOR_GREEN));
            mvprintw(i, j, " ");
        }
    }
}

void rybnik(void){
	for (int i = 10; i < 20; i++) {
        for (int j = 10; j < 20; j++) {
            attrset(COLOR_PAIR(COLOR_WHITE*10+COLOR_BLUE));
            mvprintw(i, j, "~");
        }
    }
}
*/

void pole_init(int i, int b, int f, int p, const char * s){
	pole_list[i].bg=b;
	pole_list[i].fg=f;
	pole_list[i].perm=p;
	pole_list[i].symbol=s;
}

void hrac_init(int i, int f, role r, const char * s, int xc, int yc){	
	hrac_list[i].fg=f;
	hrac_list[i].rol=r;
	hrac_list[i].symbol=s;
	hrac_list[i].cd.x=xc;
	hrac_list[i].cd.y=yc;
	hrac_list[i].live=1;
}

void attack(direction d, role r){
	int dira = 0;
	int dirb = 0;
	switch (d) {
		case left:
			dira-=1;
			dirb+=1;
			break;
		case right:
			dira+=1;
			dirb+=1;
			break;
		case up:
			dira-=1;
			dirb+=0;
			break;
		case down:
			dira+=1;
			dirb+=0;
			break;
	}
	switch (r) {
		case knife:
			break;
		case bomb:
			break;
		case shotgun:
			break;
		case sniper:
			for(int i=1;i<5;i++){
				if((hrac_list[i].cd.x%X - hrac_list[0].cd.x)*dira > 0 && hrac_list[i].cd.y%Y == hrac_list[0].cd.y * (0+dirb)){
					hrac_list[i].live = 0;
				}else if((hrac_list[i].cd.x%X - hrac_list[0].cd.x)*dira > 0 && hrac_list[i].cd.y%Y == hrac_list[0].cd.y * (0+dirb)){
					hrac_list[i].live = 0;
				}else if((hrac_list[i].cd.y%Y - hrac_list[0].cd.y)*dira > 0 && hrac_list[i].cd.x%X == hrac_list[0].cd.x * (1-dirb)){
					hrac_list[i].live = 0;
				}else if((hrac_list[i].cd.y%Y - hrac_list[0].cd.y)*dira > 0 && hrac_list[i].cd.x%X == hrac_list[0].cd.x * (1-dirb)){
					hrac_list[i].live = 0;
				}
			}
			break;
	}
	for(int i=1;i<5;i++){
		if((hrac_list[i].cd.x%X - hrac_list[0].cd.x)*dira > 0 && hrac_list[i].cd.y%Y == hrac_list[0].cd.y * (0+dirb)){
			hrac_list[i].live = 0;
		}else if((hrac_list[i].cd.x%X - hrac_list[0].cd.x)*dira > 0 && hrac_list[i].cd.y%Y == hrac_list[0].cd.y * (0+dirb)){
			hrac_list[i].live = 0;
		}else if((hrac_list[i].cd.y%Y - hrac_list[0].cd.y)*dira > 0 && hrac_list[i].cd.x%X == hrac_list[0].cd.x * (1-dirb)){
			hrac_list[i].live = 0;
		}else if((hrac_list[i].cd.y%Y - hrac_list[0].cd.y)*dira > 0 && hrac_list[i].cd.x%X == hrac_list[0].cd.x * (1-dirb)){
			hrac_list[i].live = 0;
		}
	}
}
     
int main()
{
	setlocale(LC_ALL, "");
	int ch;
      
	initscr();
	noecho();
	nocbreak();
	raw();
	if(has_colors())
	start_color();

/* nezapomenout na obnoveni nastaveni terminalu! */
	atexit(obnovTerminal);
	pairInitialization();
	
	pole_init(0, COLOR_GREEN, COLOR_GREEN, 1, " ");
	pole_init(1, COLOR_BLUE, COLOR_WHITE, 1, "~");
	pole_init(2, COLOR_MAGENTA, COLOR_RED, 0, "#");
	
	hrac_init(0, COLOR_RED, knife, "J", X/2, Y/2);
	hrac_init(1, COLOR_RED, bomb, "B", 15, 20);
	hrac_init(2, COLOR_RED, shotgun, "C", 40, 25);
	hrac_init(3, COLOR_RED, knife, "D", 10, 10);
	hrac_init(4, COLOR_RED, bomb, "E", 38, 22);
	
	vypis_mapu();
    refresh();
    keypad(stdscr, TRUE);
    coords hero;
    hero.x = X/2;
    hero.y = Y/2;
	do {
		ch = getc(stdin);
		switch (ch) {
        case 97://KEY_LEFT:
            if(pole_list[mapa[hrac_list[0].cd.y][hrac_list[0].cd.x-1]].perm){
				hrac_list[0].cd.x-=1;
			}
            break;
        case 100://KEY_RIGHT:
            if(pole_list[mapa[hrac_list[0].cd.y][hrac_list[0].cd.x+1]].perm){
				hrac_list[0].cd.x+=1;
			}
            break;
        case 119://KEY_UP:
            if(pole_list[mapa[hrac_list[0].cd.y-1][hrac_list[0].cd.x]].perm){
				hrac_list[0].cd.y-=1;
			}
            break;
        case 115://KEY_DOWN:
			if(pole_list[mapa[hrac_list[0].cd.y+1][hrac_list[0].cd.x]].perm){
				hrac_list[0].cd.y+=1;
            }
            break;
        case 102://KEY_LEFT:
            attack(left, hrac_list[0].rol);
            break;
        case 104://KEY_RIGHT:
            attack(right, hrac_list[0].rol);
            break;
        case 116://KEY_UP:
            attack(up, hrac_list[0].rol);
            break;
        case 103://KEY_DOWN:
            attack(down, hrac_list[0].rol);
            break;
        }
		usleep(100);
		vypis_mapu();
		for(int i=0;i<5;i++){
			if(hrac_list[i].live){
				attrset(COLOR_PAIR(hrac_list[i].fg*10+pole_list[mapa[hrac_list[i].cd.y%Y][hrac_list[i].cd.x%X]].bg));
				mvprintw(hrac_list[i].cd.y%Y, hrac_list[i].cd.x%X, hrac_list[i].symbol);
			}
		}/*
		for(int i=1;i<5;i++){
			hrac_list[i].cd.y+=1;
		}*/
		attrset(COLOR_PAIR(0));
		mvprintw(Y+1, X+1, "");
		refresh();
		
	} while (ch != 'q');

	return EXIT_SUCCESS;
}
/*------------------------------------------------*/
