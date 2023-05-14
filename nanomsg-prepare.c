#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/bus.h>

#define DELKA_ZPRAVY 6
#define PORT 1234

#define ID_ZPRAVY 0x0

#define ID_HRACE 0x1
#define BARVA 0x2
#define ZBRAN 0x3
#define X_SOURADNICE 0x4
#define Y_SOURADNICE 0x5

#define VYSTREL 0x2

#define BODY_1 0x2
#define BODY_2 0x3

void main(void)
{
	int sock;
	sock = nn_socket(AF_SP, NN_BUS);
	
	nn_bind(sock, "tcp://*:1234");
	
	sleep(1);	// wait for peers to bind
	int pocet_adres;
	printf("Zadejte pocet pocitacu: ");
	scanf("%d", &pocet_adres);
	printf("\n%d\n", pocet_adres);
	int ip_adresy[pocet_adres][4];
	char aktualni_adresa[20];
	int *a_a;
	for(int i=0;i<pocet_adres;i++)
	{
		printf("Zadejte adresu %d. pocitace: ", i+1);
		a_a = &ip_adresy[i][0];
		scanf("%d.%d.%d.%d", a_a, a_a+1, a_a+2, a_a+3);
	}
	
	for(int i=0;i<pocet_adres;i++)
	{
		a_a = &ip_adresy[i][0];
		for(int j=0;j<20;j++)
		{
			aktualni_adresa[j]=0;
		}
		sprintf(aktualni_adresa, "tcp://%d.%d.%d.%d:%d" , ip_adresy[i][0], ip_adresy[i][1], ip_adresy[i][2], ip_adresy[i][3], PORT);
		printf("%s\n", aktualni_adresa);
		printf("NN_CONNECT RETURN: %d \n", nn_connect(sock, aktualni_adresa));
	}

	
	sleep(5);	// wait for connections
	
	int to = 100;
	nn_setsockopt(sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to));
	char message[DELKA_ZPRAVY];
	message[ID_ZPRAVY] = 0;
	message[ID_HRACE] = 'J';
	message[BARVA] = 0b001;
	message[ZBRAN] = 2;
	message[X_SOURADNICE] = 30;
	message[Y_SOURADNICE] = 15;
	// char message[30];
	while(1)
	{
		// SEND
		/*
		printf("Zadejte zpravu: ");
		scanf("%s", message);
		
		int sz_n = strlen(message) + 1;*/
		
		nn_send(sock, &message, DELKA_ZPRAVY+1, 0);    
		
		// RECV
		char *buf = NULL;
		int recv = nn_recv(sock, &buf, NN_MSG, 0);
		while(recv>=0)
		{
			printf("FRIEND: %s \n", buf);
			nn_freemsg(buf);
			recv = nn_recv(sock, &buf, NN_MSG, 0);
		}
		sleep(0.1);
	}
	
	nn_shutdown(sock, 0);
}

