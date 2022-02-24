/* librairies standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

void construire_message(char *message, char motif, int lg)
{
	int i;
	for (i = 0; i < lg; i++)
		message[i] = motif;
}

void afficher_message(char *message, int lg)
{
	int i;
	printf("Message construit : ");
	for (i = 0; i < lg; i++)
		printf("%c", message[i]);
	printf("\n");
}

void source_udp(int port, char *host, int nb_message, int lg_M)
{
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("Echec de creation du socket \n");
		exit(1);
	}

	// Construction de l'adresse du socket destinataire
	struct hostent *hp;
	struct sockaddr_in adr_distant;
	int lg_adr_distant = sizeof(adr_distant);
	memset((char *)&adr_distant, 0, lg_adr_distant);
	adr_distant.sin_family = AF_INET;
	adr_distant.sin_port = htons(port);

	if ((hp = gethostbyname(host)) == NULL)
	{
		printf("erreur gethostbyname\n");
		exit(1);
	}
	memcpy((char *)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

	// Emission du message au destinaire
	for (int i = 0; i < nb_message; i++)
	{
		// Creation message
		char *message = malloc(lg_M * sizeof(char));
		construire_message(message, (char)(i % 26 + 97), lg_M);
		// Envoi du message
		int temp;
		if ((temp = sendto(sock, message, lg_M, 0, ((struct sockaddr *)&adr_distant), lg_adr_distant)) == -1)
		{
			printf("Echec du sendto\n");
			exit(1);
		}
		free(message);
	}

	// Destruction du socket
	if (close(sock) == -1)
	{
		printf("Echec de destructon du socket\n");
		exit(1);
	}
}

void puit_udp(int port, int lg_M, int nb_max) {
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("Echec de creation du socket\n");
		exit(1);
	}
	struct sockaddr_in adr_local;
	int lg_adr_local = sizeof(adr_local);

	// Construction de l’adresse du socket
	memset((char *)&adr_local, 0, lg_adr_local);
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(port);
	adr_local.sin_addr.s_addr = INADDR_ANY;

	// Association de l'adresse socket et représentation interne
	if ((bind(sock, (struct sockaddr *)&adr_local, lg_adr_local)) == -1)
	{
		printf("Echec du bind\n");
		exit(1);
	}

	// Réception du message
	char *pmesg = malloc(sizeof(char) * lg_M);
	struct sockaddr_in padr_em;
	int plg_adr_em = sizeof(padr_em);

	int compteur = 0;
	// nb_max == -1 signifie que nb_buf_appli est infini
	while ((nb_max == -1) || (compteur < nb_max))
	{
		int temp;
		compteur++;
		if ((temp = recvfrom(sock, pmesg, lg_M, 0, ((struct sockaddr *)&padr_em), &plg_adr_em)) == -1)
		{
			printf("Echec de réception\n");
		}
		afficher_message(pmesg, lg_M);
	}

	// Destruction du socket
	if (close(sock) == -1)
	{
		printf("Echec de destructon du socket\n");
		exit(1);
	}
}

void source_tcp(int port, char *host, int nb_message, int lg_M)
{
	// Creation du socket local
	int sock;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Echec de creation du socket\n");
		exit(1);
	}

	// Construction de l'adresse du socket destinataire
	struct hostent *hp;
	struct sockaddr_in adr_distant;
	int lg_adr_distant = sizeof(adr_distant);
	memset((char *)&adr_distant, 0, lg_adr_distant);
	adr_distant.sin_family = AF_INET;	// domaine Internet
	adr_distant.sin_port = htons(port); // n° de port

	if ((hp = gethostbyname(host)) == NULL)
	{
		printf("erreur gethostbyname\n");
		exit(1);
	}
	memcpy((char *)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

	// Demande de connexion
	if ((connect(sock, ((struct sockaddr *)&adr_distant), lg_adr_distant)) == -1)
	{
		printf("Echec de connexion\n");
		exit(1);
	}

	// Emission du message au destinaire
	for (int i = 0; i < nb_message; i++)
	{
		// Creation message
		char *message = malloc(lg_M * sizeof(char));
		construire_message(message, (char)(i % 26 + 97), lg_M);

		// Envoi du message
		int temp;
		if ((temp = write(sock, message, lg_M)) == -1)
		{
			printf("Echec du write\n");
			exit(1);
		}
		free(message);
	}

	// Destruction du socket
	if (close(sock) == -1)
	{
		printf("Echec de destructon du socket\n");
		exit(1);
	}
}

void puit_tcp(int port, int lg_M, int nb_max)
{
	// Creation du socket local
	int sock, sock_bis;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Echec de creation du socket\n");
		exit(1);
	}

	struct sockaddr_in adr_local;
	int lg_adr_local = sizeof(adr_local);

	// Construction de l’adresse du socket
	memset((char *)&adr_local, 0, lg_adr_local);
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(port);
	adr_local.sin_addr.s_addr = INADDR_ANY;

	// Association de l'adresse socket et représentation interne
	if ((bind(sock, (struct sockaddr *)&adr_local, lg_adr_local)) == -1)
	{
		printf("Echec du bind\n");
		exit(1);
	}

	// Dimensionnement de la file de demandes de connexion avec la primitive listen
	if ((listen(sock, nb_max)) == -1)
	{
		printf("Echec du listen\n");
		exit(1);
	}

	// Mise en état d’acceptation de demande de connexion avec la primitive accept
	struct sockaddr_in padr_em;
	int plg_adr_em = sizeof(padr_em);
	if ((sock_bis = accept(sock, ((struct sockaddr *)&padr_em), &plg_adr_em)) == -1)
	{
		printf("Echec d'acceptation\n");
		exit(1);
	}

	// Réception du message
	char *pmesg = malloc(sizeof(char) * lg_M);
	int temp;
	while (temp = read(sock_bis, pmesg, lg_M) != 0)
	{
		if (temp < 0)
			printf("Echec de réception\n");
		afficher_message(pmesg, lg_M);
	}

	// Destruction des sockets
	if (close(sock) == -1)
	{
		printf("Echec de destructon du socket\n");
		exit(1);
	}
	if (close(sock_bis) == -1)
	{
		printf("Echec de destructon du socket\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = 10; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int nb_mess_modif = 0; // Permet de savoir si nb_message a été modifié
	int source = -1;	 /* 0=puits, 1=source */
	int protocol = 1;	 // 0 = UDP, 1 = TCP (par défaut)
	int port = (atoi(argv[argc - 1]));
	char *host = argv[argc - 2];
	int lg_M = 30; // Longueur des messages à envoyer, par défaut : 30

	while ((c = getopt(argc, argv, "upn:l:s")) != -1)
	{
		switch (c)
		{
		case 'p':
			if (source == 1)
			{
				printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0)
			{
				printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
				exit(1);
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			nb_mess_modif = 1;
			break;

		case 'u':
			protocol = 0;
			break;

		case 'l':
			lg_M = atoi(optarg);
			break;

		default:
			printf("usage: cmd [-p|-s][-u][-n ##][-l ##]\n");
			break;
		}
	}

	if (source == -1)
	{
		printf("usage: cmd [-p|-s][-u][-n ##]\n");
		exit(1);
	}

	if (source == 1)
	{
		printf("on est dans la source\n");
		switch (protocol)
		{
		case 0:
			source_udp(port, host, nb_message, lg_M);
			break;
		case 1:
			source_tcp(port, host, nb_message, lg_M);
			break;

		default:
			printf("Mauvais protocole");
			break;
		}
	}
	else
	{
		printf("on est dans le puit\n");
		switch (protocol)
		{
		case 0:
			if (!nb_mess_modif) {
				nb_message = -1; // Le nombre de messages en réception n'a pas été modifié donc il est infini par défaut (désigné par -1)
			}
			puit_udp(port, lg_M, nb_message);
			break;

		case 1:
			puit_tcp(port, lg_M, nb_message);
			break;

		default:
			printf("Mauvais protocole");
			break;
		}
	}

	if (nb_message != -1)
	{
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	}
	else
	{
		if (source == 1)
		{
			nb_message = 10;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		}
		else
			printf("nb de tampons à envoyer = infini\n");
	}
	return 0;
}
