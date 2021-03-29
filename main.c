#include "joypi.h"
#include "puissance4.h"

extern unsigned char colonne_choisie;

PI_THREAD(afficher_jeu)
{
	while(1){
		dessiner_grille_LED(grille_jeu);
		//afficher_grille(grille_jeu);
		delay(100);
		dessiner_grille_LED(grille_J1);
		//afficher_grille(grille_J1);
		delay(100);
	}
}

static int demande_action(int *coup)
{
	/*
		* Demande l'action à effectuer au joueur courant.
		* S'il entre un chiffre, c'est qu'il souhaite jouer.
		* S'il entre la lettre « Q » ou « q », c'est qu'il souhaite quitter.
		* S'il entre autre chose, une nouvelle saisie sera demandée.
	*/
	while(digitalRead(LOWER_BUTTON_PORT));		//on attend que le bouton soit appuyé
	switch(colonne_choisie){
	case 0x80:
		*coup=1;
		break;

	case 0x40:
		*coup=2;
		break;

	case 0x20:
		*coup=3;
		break;

	case 0x10:
		*coup=4;
		break;

	case 0x08:
		*coup=5;
		break;

	case 0x04:
		*coup=6;
		break;

	case 0x02:
		*coup=7;
		break;

	default:
		break;
	}
	while(!digitalRead(LOWER_BUTTON_PORT));	//on attend que le bouton soit relaché
	delay(50);
	return ACT_JOUER;
}

int main(void)
{
	//joypi.c

	wiringPiSetup();																	//setup wiringPi
	init_gpio();																		//initialise les ports en I/O

	wiringPiISR(LOWER_BUTTON_PORT, INT_EDGE_FALLING, bouton_bas);		//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton bas
	wiringPiISR(LEFT_BUTTON_PORT, INT_EDGE_FALLING, bouton_gauche);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton gauche
	wiringPiISR(RIGHT_BUTTON_PORT, INT_EDGE_FALLING, bouton_droit);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton droit
	wiringPiISR(UPPER_BUTTON_PORT, INT_EDGE_FALLING, bouton_haut);		//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton haut
	init_MAX7219();																	//initialise la matrice LED
	clear_LED();

	//puissance4.c

	int statut;
	char jeton = J1_JETON;
	int njoueur;

	initialise_grille();
	//afficher_grille(grille_jeu);
	njoueur = demande_nb_joueur();
	printf("nb de joueurs demandés\n"); //debug
	piThreadCreate(afficher_jeu);
	printf("thread créé\n"); //debug
	delay(1000);
	write_MAX7219(0x01, colonne_choisie);

	if (!njoueur)
		return EXIT_FAILURE;

	while (1)
	{
		struct position pos;
		int action;
		int coup;

		if (njoueur == 1 && jeton == J2_JETON)
		{
			coup = ia();
			printf("Joueur 2 : %d\n", coup);
			calcule_position(coup - 1, &pos);
		}
		else
		{
			printf("Joueur %d : \n", (jeton == J1_JETON) ? 1 : 2);
			action = demande_action(&coup);

			if (!coup_valide(coup))
			{
				fprintf(stderr, "Vous ne pouvez pas jouer à cet endroit\n");
				continue;
			}

			calcule_position(coup - 1, &pos);
		}
		printf("coup joué : %d\n", coup);
		animation_grille_LED(grille_jeu, coup);
		grille_jeu[pos.colonne][pos.ligne] = jeton;
		separer_grille(grille_jeu, grille_J1);
		//afficher_grille(grille_jeu);
		statut = statut_jeu(&pos, jeton);

		if (statut != STATUT_OK)
			break;

		jeton = (jeton == J1_JETON) ? J2_JETON : J1_JETON;    
	}

	if (statut == STATUT_GAGNE)
		printf("Le joueur %d a gagné\n", (jeton == J1_JETON) ? 1 : 2);
	else if (statut == STATUT_EGALITE)
		printf("Égalité\n");

	write_MAX7219(SHUTDOWN,0x00);
	write_MAX7219(SHUTDOWN,0x00);

	return 0;
}