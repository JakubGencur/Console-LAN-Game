/*----------------------------------------Knihovny---------------------------------------*/
// Standard libraris
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Threads
#include <pthread.h>

// IO libraries
#include <ncurses.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

// Network communication
#include <nanomsg/nn.h>
#include <nanomsg/bus.h>

/*------------------------------------------Makra:---------------------------------------*/
// Obecné
#define VYSKA 30
#define SIRKA 60
#define MAX_HRACU 10
#define POCET_TYPU 3
#define POCET_ZBRANI 8
#define DOSAH 7
// Zobrazení
#define POCET_BAREV 8
// Síť
#define ZNAKY_IP_ADRESY 27
#define PORT 1234
// Vlastnosti zprávy
#define DELKA_ZPRAVY 6
//   Adresy v rámci zprávy
#define ID_ZPRAVY 0x0
#define ZPRAVA_PRUBEH 0x1
#define ZPRAVA_ZACATEK 0x2
#define ZPRAVA_KONEC 0x3

#define ID_HRACE 0x1
#define BARVA 0x2
#define ZBRAN 0x3
#define X_SOURADNICE 0x4
#define Y_SOURADNICE 0x5

#define VYSTREL 0x2

#define BODY_1 0x2
#define BODY_2 0x3

typedef struct{
	char symbol;
	char barva;
	char souradnice[2];
	char zbran;
	char zije;
	char vystrel;
}hrac; // Proměnná hráče

typedef struct
{
	char barva_poz;
	char barva_pop;
	char pruchod;
	char symbol;
}pole; // Proměnná typu pole

/*-----------------------------------Globální proměnné:----------------------------------*/
hrac hraci[MAX_HRACU]; // Zde jsou uloženi jednotliví hráči
pole typy_poli[POCET_TYPU]; // Zde jsou uloženy typy polí
unsigned char adresar[MAX_HRACU][4]; // Zde ukládá IP adresy, není potřeba, aby byly propojeny s proměnnými hráčů.

int socket; // Globální proměnná nanomsg knihovny
char ukoncit = 0; // Aktivuje se pro ukončení smyčky
char pocet_hracu = 0; // Počet hrajících hráčů
int vyska=0, sirka=0; // Výška, šířka terminálu
char aktualni_adresa[ZNAKY_IP_ADRESY]; // Proměnná pro ukládání adresy jako řetězce (nutná pro připojení)
char zprava[DELKA_ZPRAVY]; // Proměnná pro ukládání zprávy
char schranka[DELKA_ZPRAVY]; // Proměnná pro příjem zpráv
int recv = 0; // Proměnná pro ukládání stavu přijímnných zpráv

char zbrane_nazvy[POCET_ZBRANI][11]={"Nuz", "Puska", "Brokovnice", "Granat"};// Zde jsou uloženy názvy zbraní

char zbrane[POCET_ZBRANI][DOSAH][DOSAH]={
	{
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,1,1,1,0,0},
		{0,0,1,0,1,0,0},
		{0,0,1,1,1,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0}
	},
	{
		{0,0,0,2,0,0,0},
		{0,0,0,2,0,0,0},
		{0,0,0,2,0,0,0},
		{5,5,5,0,3,3,3},
		{0,0,0,4,0,0,0},
		{0,0,0,4,0,0,0},
		{0,0,0,4,0,0,0}
	},
	{
		{0,0,0,0,0,0,0},
		{0,0,2,2,2,0,0},
		{0,5,0,2,0,3,0},
		{0,5,5,0,3,3,0},
		{0,5,0,4,0,3,0},
		{0,0,4,4,4,0,0},
		{0,0,0,0,0,0,0}
	},
	{
		{0,0,2,2,2,0,0},
		{0,0,0,0,0,0,0},
		{5,0,0,0,0,0,3},
		{5,0,0,0,0,0,3},
		{5,0,0,0,0,0,3},
		{0,0,0,0,0,0,0},
		{0,0,4,4,4,0,0}
	}
}; // Zde jsou uloženy dosahy zbraní.

int mapa[VYSKA+1][SIRKA] = {
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
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
}; // Zde je uložena mapa, jak bude vykreslena


/*---------------------------------Funkce:-----------------------------------------------*/
void vypis_mapu(void) // Vypíše mapu, jak má vypadat
{
	int typ=0;
	for (int i = 0; i < VYSKA; i++) {
        for (int j = 0; j < SIRKA; j++) {
			typ=mapa[i][j];
            attrset(COLOR_PAIR(typy_poli[typ].barva_pop*8+typy_poli[typ].barva_poz));
            mvprintw(i, j, "%c", typy_poli[typ].symbol);
        }
    }
    for(int i=0;i<=pocet_hracu;i++)
	{
		attrset(COLOR_PAIR(0b111000));
		mvprintw(32+i, 3, "%d, %c, %d, %d, %d, %d", schranka[ID_ZPRAVY], schranka[ID_HRACE], schranka[BARVA], schranka[ZBRAN], schranka[X_SOURADNICE], schranka[Y_SOURADNICE]);
	}
}

void vycisti_mapu(void) // Vypíše do celé obrazovky mezerníky
{
	attrset(COLOR_PAIR(COLOR_WHITE*POCET_BAREV+COLOR_BLACK));
	for (int i = 0; i < vyska; i++) {
        for (int j = 0; j < sirka; j++) {
            mvprintw(i, j, " ");
        }
    }
}
void umyj_schranku(void)
{
	for(int i=0;i<DELKA_ZPRAVY;i++)
	{
		schranka[i]=0;
	}
}

void pole_init(int i, char b, char f, char p, char s) // Zapíše hodnoty i-tého typu pole
{
	typy_poli[i].barva_pop=f; // Barva popředí
	typy_poli[i].barva_poz=b; // Barva pozadí
	typy_poli[i].pruchod=p;   // Průchodnost
	typy_poli[i].symbol=s;    // Symbol pozadí
}

void hrac_init(int i, char f, char r, char s, char xc, char yc) // Zapíše hodnoty i-tého hráče
{	
	hraci[i].barva=f;
	hraci[i].zbran=r;
	hraci[i].symbol=s;
	hraci[i].souradnice[0]=xc;
	hraci[i].souradnice[1]=yc;
	hraci[i].zije=1;
	hraci[i].vystrel=0;
}

void hrac_aktualizuj(char s, char v, char xc, char yc) // Zapíše hodnoty i-tého hráče
{	
	for(int i=1;i<=pocet_hracu;i++)
	{
		if(hraci[i].symbol==s)
		{
			hraci[i].souradnice[0]=xc;
			hraci[i].souradnice[1]=yc;
			if(v!=0){
				hraci[i].vystrel=v;
			}
		}
	}
}

static void obnov_terminal() // Po ukončení smyčky obnoví terminál 
{
	
	vycisti_mapu();
	// -----Odpojí a ukončí program-----
	sleep(0.1);
	//pthread_exit(NULL);
	//pthread_exit(NULL);
	
	nn_shutdown(socket, 0);
	
    printw("Obnovuji nastavení terminálu\n");
    endwin();
    echo();
}

void init_paru_barev(void) // Initializuje všechny páry barev (pár = barva pozadí + barva popředí)
{
	for (int i = 0; i < POCET_BAREV; i++) {
        for (int j = 0; j < POCET_BAREV; j++) {
            init_pair(i*POCET_BAREV+j, i, j);
        }
    }
}

void zkontroluj_vystrely(void) // Zkontroluje zda nějáký hráč vystřelil a zda  zasáhl
{
	for(int i=0;i<=pocet_hracu;i++)
	{
		for(int j;j<=pocet_hracu;j++)
		{
			if(hraci[i].vystrel && // Zkontroluje, zda hráč i vytřelil
			(3+hraci[j].souradnice[0]-hraci[i].souradnice[0]<7 && 3+hraci[j].souradnice[1]-hraci[i].souradnice[1]<7) && // Zda je hráč j v dosahu hráče j (abychom neodkazovali mimo pole zbraní)
			(zbrane
				[hraci[i].zbran] 
				[(DOSAH-1)/2-hraci[i].souradnice[1]+hraci[j].souradnice[1]]
				[(DOSAH-1)/2-hraci[i].souradnice[0]+hraci[j].souradnice[0]]==hraci[i].vystrel) // Zkontroluje zda hráč i hráče j trefil
			){
				hraci[j].zije=0; // Zabije hráče j
			}
		}
		hraci[i].vystrel = 0; // Ukončí výstřel
	}
}

void nastaveni_hrace(void) // Nastaví hodnoty podle hráčových preferencí:
{
	int nastaveni_souhlasi = 1;
	int parametr[4]={0,0,0,0}; // Proměnné, k načítání intů, a následnému převodu do charů
	
	do{
		// Zeptá se hráče a uloží zadanou hodnotu

		attrset(COLOR_PAIR(7*POCET_BAREV));
		// Symbol hráče (jeho postavy):
		vycisti_mapu();
		mvprintw(3, 3,"Napis, jaky chces mit znak (musi byt jiny, nez u dalsich hracu):");
		scanw("%c", &hraci[0].symbol);
		vycisti_mapu();
		// Barva hráčova symbolu:
		mvprintw(3, 3,"Napis jakou chces mit barvu (cervena-%d, modra-%d, cerna-%d, bila-%d, magenta-%d, zluta-%d, svetle-modra-%d):", COLOR_RED, COLOR_BLUE, COLOR_BLACK, COLOR_WHITE, COLOR_MAGENTA, COLOR_YELLOW, COLOR_CYAN);
		scanw("%d", &parametr[0]);
		hraci[0].barva = parametr[0] % POCET_BAREV;
		vycisti_mapu();
		// Zbraň, kterou použije:
		mvprintw(3, 3,"Napis svou zbran (%s-%d, %s-%d, %s-%d, %s-%d):", zbrane_nazvy[0], 0, zbrane_nazvy[1], 1, zbrane_nazvy[2], 2, zbrane_nazvy[3], 3);
		scanw("%d", &parametr[0]);
		hraci[0].zbran = parametr[0] % POCET_ZBRANI;
		vycisti_mapu();
		// Výchozí bod
		mvprintw(3, 3,"Napis, kde chces zacit x,y(1-60, 1-30):");
		scanw("%d,%d", &parametr[0], &parametr[1]);
		hraci[0].souradnice[0] = parametr[0] % SIRKA;
		hraci[0].souradnice[1] = parametr[1] % VYSKA;
		vycisti_mapu();
		
		hraci[0].zije = 1;
		
		// Získá počet ostatních hráčů a jejich ip adresy
		mvprintw(3, 3, "Napis pocet ostatnich hracu (1-%d):", MAX_HRACU);
		scanw("%d", &parametr[0]);
		pocet_hracu = parametr[0] % MAX_HRACU;
		vycisti_mapu();
		for(int k=0;k<pocet_hracu;k++)
		{
			mvprintw(3, 3,"Napis IPv4 adresu %d. hrace:", k+1);
			scanw("%d.%d.%d.%d", &parametr[0], &parametr[1], &parametr[2], &parametr[3]);
			for(int l=0;l<4;l++)
			{
				adresar[k][l] = parametr[l]%256;
			}
			vycisti_mapu();
		}
		
		// Vypíše schrnutí a zeptá se hráče, jestli chce s tímto nastavením pokračovat:
		mvprintw(3, 3, "Tvoje postava:");
		attrset(COLOR_PAIR(hraci[0].barva*POCET_BAREV));
		mvprintw(3, 3+15, "%c", hraci[0].symbol);
		attrset(COLOR_PAIR(7*POCET_BAREV));
		mvprintw(4, 5, "Tvoje zbran: %s", zbrane_nazvy[hraci[0].zbran]);
		mvprintw(5, 3, "Zacinas zde: x=%d,y=%d", hraci[0].souradnice[0], hraci[0].souradnice[1]);
		mvprintw(6, 3, "Hraje celkem %d dalsich hracu s temito adresami", pocet_hracu);
		for(int k=0;k<pocet_hracu;k++)
		{
			mvprintw(k+7, 6, "%d.%d.%d.%d", adresar[k][0], adresar[k][1], adresar[k][2], adresar[k][3]);
		}
		mvprintw(pocet_hracu+7, 3, "Souhlasi? stiskni n, pokud ne:  ");
		if(getch()=='n'){
			mvprintw(pocet_hracu+8, 3, "Nesouhlasi! Tak znovu.");
			getch();
			nastaveni_souhlasi=0;
		}
		else
		{
			nastaveni_souhlasi=1;
		}
	}while(!nastaveni_souhlasi);
	mvprintw(pocet_hracu+8, 3, "Souhlasi!");
	// Čeká na ostatní hráče, aby nevznikly chyby v komunikaci:
	mvprintw(pocet_hracu+10, 3, "Pockej na ostatni hrace, pak zaraz stisknete Enter");
	getch();	
}
char lepsi_modulo(char hodnota, int delitel, int rozdil)// Funkce, aby souřadnice nepřesahovaly rozsah mapy
{
	hodnota += rozdil;
	hodnota = hodnota%delitel;
	if(hodnota<0){
		hodnota+=delitel;
	}
	return hodnota;
}

/*----------------------------------Vedlejší vlákna--------------------------------------*/
void *klavesnice(void *) // Obsluha vstupu z klávesnice
{
	int ch;
	do{
		//usleep(100000);
		ch = 0;
		// Získá vstup z klávesnice, a zkontroluje, zda má speciální funkci
		ch = getch();
		if(ch){
			switch (ch) {
				case KEY_LEFT:
					if(typy_poli[mapa[hraci[0].souradnice[1]][hraci[0].souradnice[0]-1]].pruchod){
						hraci[0].souradnice[0]=lepsi_modulo(hraci[0].souradnice[0], SIRKA, -1);
					}
					break;
				case KEY_RIGHT:
					if(typy_poli[mapa[hraci[0].souradnice[1]][hraci[0].souradnice[0]+1]].pruchod){
						hraci[0].souradnice[0]=lepsi_modulo(hraci[0].souradnice[0], SIRKA, 1);
					}
					break;
				case KEY_UP:
					if(typy_poli[mapa[hraci[0].souradnice[1]-1][hraci[0].souradnice[0]]].pruchod){
						hraci[0].souradnice[1]=lepsi_modulo(hraci[0].souradnice[1], VYSKA, -1);
					}
					break;
				case KEY_DOWN:
					if(typy_poli[mapa[hraci[0].souradnice[1]+1][hraci[0].souradnice[0]]].pruchod){
						hraci[0].souradnice[1]=lepsi_modulo(hraci[0].souradnice[1], VYSKA, 1);
					}
					break;
				case 'a':
					hraci[0].vystrel=5;
					/*attrset(COLOR_PAIR(typy_poli[mapa[hraci[0].souradnice[1]][hraci[0].souradnice[0]-1]].barva_poz));
					mvprintw(hraci[0].souradnice[1], hraci[0].souradnice[0]-1, ".");
					refresh();
					usleep(100000);
					attrset(COLOR_PAIR(typy_poli[mapa[hraci[0].souradnice[1]][hraci[0].souradnice[0]-2]].barva_poz));
					mvprintw(hraci[0].souradnice[1], hraci[0].souradnice[0]-2, ".");
					attrset(COLOR_PAIR(typy_poli[mapa[hraci[0].souradnice[1]+1][hraci[0].souradnice[0]-2]].barva_poz));
					mvprintw(hraci[0].souradnice[1]+1, hraci[0].souradnice[0]-2, ".");
					attrset(COLOR_PAIR(typy_poli[mapa[hraci[0].souradnice[1]-1][hraci[0].souradnice[0]-2]].barva_poz));
					mvprintw(hraci[0].souradnice[1]-1, hraci[0].souradnice[0]-2, ".");
					refresh();
					usleep(100000);*/
					break;
				case 'd':
					hraci[0].vystrel=3;
					break;
				case 'w':
					hraci[0].vystrel=2;
					break;
				case 's':
					hraci[0].vystrel=4;
					break;
				case 'e':
					hraci[0].vystrel=1;
					break;
				case 'q':
					ukoncit = 1;
					break;
			}
		}
	}while(!ukoncit);
}

void *pripojeni(void *)
{
	// Připojí se k ostatním počítačům, nastaví spojení
	
	/*
	// Zde je část kodu, kterou lze použít pro vytvoření botů
	for(int i;i<pocet_hracu;i++)
	{
		hrac_init(i+1, 0, 2, 'a'+2*i, 30+3*i, 16+3*i);
	}*/
	// Získává data od ostaatních počítačů, odesílá vlastní
	while(!ukoncit)
	{
		zprava[ID_ZPRAVY] = ZPRAVA_PRUBEH;
		zprava[ID_HRACE] = hraci[0].symbol;
		zprava[VYSTREL] = hraci[0].vystrel;
		zprava[X_SOURADNICE] = hraci[0].souradnice[0];
		zprava[Y_SOURADNICE] = hraci[0].souradnice[1];
		/*mvprintw(30, 3, "               ");
		mvprintw(30, 3, "%d, %d", hraci[0].souradnice[0], hraci[0].souradnice[1]);*/
		nn_send(socket, &zprava, DELKA_ZPRAVY, 0);
		recv = nn_recv(socket, &schranka, sizeof(schranka), NN_DONTWAIT);
		while(recv>=0)
		{
			if(schranka[ID_ZPRAVY]==ZPRAVA_PRUBEH)
			{
				hrac_aktualizuj(schranka[ID_HRACE], schranka[VYSTREL], schranka[X_SOURADNICE], schranka[Y_SOURADNICE]);
			}
			umyj_schranku();
			recv = nn_recv(socket, &schranka, sizeof(schranka), NN_DONTWAIT);
		}
		/*
		// Zde je smyčka, kterou lze použít pro správu botů
		for(int i = 1;i<=pocet_hracu;i++)
		{
			if(hraci[i].zije)
			{
				int k=rand()%4;
				hraci[i].vystrel=rand()%2;
				switch(k) 
				{
					case 0:
						if(typy_poli[mapa[hraci[i].souradnice[1]][hraci[i].souradnice[0]-1]].pruchod){
							hraci[i].souradnice[0]=lepsi_modulo(hraci[i].souradnice[0], SIRKA, -1);
						}
						break;
					case 1:
						if(typy_poli[mapa[hraci[i].souradnice[1]][hraci[i].souradnice[0]+1]].pruchod){
							hraci[i].souradnice[0]=lepsi_modulo(hraci[i].souradnice[0], SIRKA, 1);
						}
						break;
					case 2:
						if(typy_poli[mapa[hraci[i].souradnice[1]-1][hraci[i].souradnice[0]]].pruchod){
							hraci[i].souradnice[1]=lepsi_modulo(hraci[i].souradnice[1], VYSKA, -1);
						}
						break;
					case 3:
						if(typy_poli[mapa[hraci[i].souradnice[1]+1][hraci[i].souradnice[0]]].pruchod){
							hraci[i].souradnice[1]=lepsi_modulo(hraci[i].souradnice[1], VYSKA, 1);
						}
						break;
				}
			}
		}*/
		sleep(0.03);
	}
}

/*--------------------------------------Main:--------------------------------------------*/

int main(void){
	// -----Zavedení programu-----
	setlocale(LC_ALL, "");
	
	// Zavede ncurses knihovnu
	initscr();
	// Zkontroluje velikost emulátoru
	getmaxyx(stdscr,vyska,sirka);
	if(vyska<VYSKA || sirka<SIRKA){
		printf("Vas emulator je prilis maly. Zvetste ho, nebo zmensete font.");
		return 1;
	}
	//noecho();
	nocbreak();
	raw();
	
	// Zkontroluje barvy v terminalu a zapne je.
	if(!has_colors()){
		printf("Vas emulator nepodporuje barevny vypis. Zvolte, prosim, jiny.");
		return 2;
	}
	start_color();
	init_paru_barev(); // Nastaví jednotlivé páry barvy textu, pozadí
	
	atexit(obnov_terminal); // Po ukončení obnoví terminál, nefunguje při erroru
	
	// Nastaví, jak budou vypadat jednotlivá pole z mapy
	pole_init(0, COLOR_GREEN, COLOR_GREEN, 1, ' ');
	pole_init(1, COLOR_BLUE, COLOR_WHITE, 1, '~');
	pole_init(2, COLOR_MAGENTA, COLOR_RED, 0, '#');
	
	// -----Nastaví parametry zadané hráčem------
	// nastaveni_hrace();
	hrac_init(0, 4, 2, 'J', 5, 5);
	pocet_hracu = 1;
	adresar[0][0]=192;
	adresar[0][1]=168;
	adresar[0][2]=1;
	adresar[0][3]=104;
	
	noecho(); // Vypne vypisování vstupu z klávesnice
	
	// -----Zavede vedlejší vlákna - klávesnice a síťová komunikace-----
	keypad(stdscr, TRUE);
	
	// Zavede síťovou komunikaci	
	socket = nn_socket(AF_SP, NN_BUS);
	char localhost[15];
	sprintf(localhost, "tcp://*:%d", PORT);
	nn_bind(socket, localhost);
	sleep(2);
	// Připojí se k ostatním počítačům
	for(int i=0;i<pocet_hracu;i++)
	{
		for(int j=0;j<ZNAKY_IP_ADRESY;j++)
		{
			aktualni_adresa[j]=0;
		}
		sprintf(aktualni_adresa, "tcp://%d.%d.%d.%d:%d" , adresar[i][0], adresar[i][1], adresar[i][2], adresar[i][3], PORT);
		nn_connect(socket, aktualni_adresa);
	}
	// Čeká na inicializační zprávy ostatních a odesílá je.
	int vsichni_pripojeni=0;
	for(int i=1;i<=pocet_hracu;i++)
	{
		hraci[i].zije=0;
	}
	zprava[ID_ZPRAVY] = ZPRAVA_ZACATEK;
	zprava[ID_HRACE] = hraci[0].symbol;
	zprava[BARVA] = hraci[0].barva;
	zprava[ZBRAN] = hraci[0].zbran;
	zprava[X_SOURADNICE] = hraci[0].souradnice[0];
	zprava[Y_SOURADNICE] = hraci[0].souradnice[1];
	while(!vsichni_pripojeni)
	{
		//Čte, dokud nejsou všichni hráči připojení
		// sleep(0.2);
		nn_send(socket, &zprava, DELKA_ZPRAVY, 0);// = NULL;
		recv = nn_recv(socket, &schranka, sizeof(schranka), 0);
		while(recv>=0)
		{
			{
				int k=1;
				int i=0;
				while(k && i<=pocet_hracu && schranka[ID_HRACE] != hraci[i].symbol)
				{
					if(!hraci[i].zije)
					{
						hraci[i].zije=1;
						hrac_init(i, schranka[BARVA], schranka[ZBRAN], schranka[ID_HRACE], schranka[X_SOURADNICE], schranka[Y_SOURADNICE]);
						k=0;
					}
					i++;
				}
				mvprintw(3, 1, "%d, %c, %d, %d, %d, %d", schranka[ID_ZPRAVY], schranka[ID_HRACE], schranka[BARVA], schranka[ZBRAN], schranka[X_SOURADNICE], schranka[Y_SOURADNICE]);
				refresh();
			}
			umyj_schranku();
			recv = nn_recv(socket, &schranka, sizeof(schranka), 0);
		}
		vsichni_pripojeni = 1;
		for(int i=1;i<pocet_hracu+1;i++)
		{
			vsichni_pripojeni*=hraci[i].zije;
		}
	}
	vycisti_mapu();
	mvprintw(1,1,"AHOJ");
	refresh();
	sleep(1);
	// Pošle pět zpráv, pro případ, že ne všichni je stihli příjmout
	/*for(int i=1;i<=5;i++)
	{
		nn_send(socket, &zprava, DELKA_ZPRAVY, 0);
		sleep(0.5);
	}*/
	
	// Proměnné vláken
	pthread_t vlakno_klavesnice;
	pthread_t vlakno_pripojeni;
	
	// Vyvoření vláken
	pthread_create(&vlakno_klavesnice, NULL, klavesnice, NULL);
	pthread_create(&vlakno_pripojeni, NULL, pripojeni, NULL);
	// -----Vlastní hra-----
	
	vycisti_mapu();
	mvprintw(1,1,"AHOJ2");
	refresh();
	sleep(1);
	do {
		usleep(100000);
		// vycisti_mapu(); // Vyčistí pole k zobrazení
		vypis_mapu(); // Vypíše hrací pole
		zkontroluj_vystrely(); // Zkontroluje zabití
		for(int i=0;i<=pocet_hracu;i++){
			// Vypíše všechny postavy hráčů
			if(hraci[i].zije){
				attrset(COLOR_PAIR(hraci[i].barva*8+typy_poli[mapa[hraci[i].souradnice[1]][hraci[i].souradnice[0]]].barva_poz));
				mvprintw(hraci[i].souradnice[1], hraci[i].souradnice[0], "%c", hraci[i].symbol);
			}
		}
		for(int i=0;i<=pocet_hracu;i++)
		{
			mvprintw(32+i, 3, "%d, %d, %d, %d, %d, %d, %d      ", i, hraci[i].symbol, hraci[i].zbran, hraci[i].barva, hraci[i].souradnice[1], hraci[i].souradnice[0], hraci[i].vystrel);
		}
		// Vypíše text mimo pole, aby kurzor v poli nepřekážel
		attrset(COLOR_PAIR(COLOR_WHITE*POCET_BAREV+COLOR_BLACK));
		mvprintw(VYSKA+1, SIRKA+1, " ");
		refresh();
		
	}while(!ukoncit);
	
	return EXIT_SUCCESS;
}
