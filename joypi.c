#include "joypi.h"

unsigned char colonne_choisie = 0x80;	//permet la selection du coup à jouer

//permet d'écrire des données dans un registre du MAX7219 (voir le datasheet du MAX7219 pour plus d'informations)
void write_MAX7219(unsigned char regAddr, unsigned char regData)
{
	//printf("%x %x\n",regAddr&0xff , regData&0xff );
	unsigned char SPICount;
	unsigned char SPIData;

	while(!digitalRead(LOAD_PORT));	//empêche l'appel simultané de deux programmes (dû au fork de wiringPiISR) (équivalent à l'appel while(MAX7219_in_use);)

	digitalWrite(LOAD_PORT, 1);
	digitalWrite(CLK_PORT, 0);
	SPIData=regAddr;
	digitalWrite(LOAD_PORT, 0);

	for(SPICount=0;SPICount<8;SPICount++){
		if(SPIData&0x80){
			digitalWrite(MOSI_PORT,1);
			delay(3);
		} 
		else{
			digitalWrite(MOSI_PORT,0);
			delay(3);
		} 
		digitalWrite(CLK_PORT, 1);
		digitalWrite(CLK_PORT, 0);
		SPIData<<=1;
	}

	SPIData=regData;
	for(SPICount=0;SPICount<8;SPICount++){
		if(SPIData&0x80){
			digitalWrite(MOSI_PORT,1);
			delay(3);
		} 
		else{
			digitalWrite(MOSI_PORT,0);
			delay(3);
		}
		digitalWrite(CLK_PORT, 1);
		digitalWrite(CLK_PORT, 0);
		SPIData<<=1;
	}
	digitalWrite(LOAD_PORT, 1);
	digitalWrite(MOSI_PORT,0);
}

//set le mode des différents ports gpio utilisés
void init_gpio()
{	
	pinMode(MOSI_PORT, OUTPUT);
	pinMode(CLK_PORT, OUTPUT);
	pinMode(LOAD_PORT, OUTPUT);
	digitalWrite(LOAD_PORT, 1);			//permet de terminer un envoi mal effectué et ainsi de réinitialiser le MAX7219
	pinMode(LOWER_BUTTON_PORT, OUTPUT);
	pinMode(LEFT_BUTTON_PORT, OUTPUT);
	pinMode(RIGHT_BUTTON_PORT, OUTPUT);
	pinMode(UPPER_BUTTON_PORT, OUTPUT);
}

//initialise la matrice LED en envoyant les données nécessaires dans les registres du MAX7219
void init_MAX7219()
{
	//l'envoi double des données dans les registres semble permettre d'initialiser correctement la matrice LED à tous les coups
	write_MAX7219(DECODE_MODE, 0x00);	//désactive le decode-mode
	write_MAX7219(DECODE_MODE, 0x00);
	write_MAX7219(INTENSITY, 0x01);		//met l'intensité des LED à 1
	write_MAX7219(INTENSITY, 0x01);
	write_MAX7219(SCAN_LIMIT, 0x07);		//non utilisé
	write_MAX7219(SCAN_LIMIT, 0x07);
	write_MAX7219(SHUTDOWN,0x01);			//active la matrice LED
	write_MAX7219(SHUTDOWN,0x01);
	write_MAX7219(DISPLAY_TEST, 0x00);	//désactive la fonction d'affichage "test"
	write_MAX7219(DISPLAY_TEST, 0x00);
}

//permet de retranscrire la grille de puissance 4 sur la matrice LED
void dessiner_grille_LED(char grille[7][6])
{
	//printf("On entre dans dessiner_grille_LED\n");

	unsigned char data=0x00;					//ligne LED

	for(int j=0;j<6;j++){
		for(int i=0;i<7;i++){
			data<<=1;								//décale de 1 bit vers la gauche
			if(grille[i][j]!=' ') data+=1;
			// printf("data=\'%x\'\n", data&0xff);
		}
		data<<=1;									//permet d'afficher la grille de puissance 4 dans le coin gauche de la matrice LED
		write_MAX7219((unsigned char) (j+3), data);
		// printf("%x %x\n",((unsigned char) (j+3))&0xff , data&0xff );
		data=0x00;
	}
}

//permet d'éteindre toutes les LED (ne désactive pas la matrice LED)
void clear_LED()
{
	write_MAX7219(0x01, 0x00);	
	write_MAX7219(0x02, 0x00);
	write_MAX7219(0x03, 0x00);
	write_MAX7219(0x04, 0x00);
	write_MAX7219(0x05, 0x00);
	write_MAX7219(0x06, 0x00);
	write_MAX7219(0x07, 0x00);
	write_MAX7219(0x08, 0x00);
}

//permet d'obtenir une grille ne contenant que le joueur 1
void separer_grille(char grille[7][6], char grille_J1[7][6])
{
	for(int i=0;i<7;i++){
		for(int j=0;j<7;j++){
			if(grille[i][j]=='X') grille_J1[i][j]=' ';
			else grille_J1[i][j]=grille[i][j];
		}
	}
}

//fonction appellée lors de l'appel au bouton haut, refresh la matrice LED
void bouton_haut()
{
	//printf("Appui sur le bouton bas\n");
	while(!digitalRead(UPPER_BUTTON_PORT));	//on attend que le bouton soit relaché
	clear_LED();
}

void bouton_bas()
{

}

//fonction appellée lors de l'appel au bouton gauche, "déplace" la LED de la première ligne vers la gauche, ce qui permet de selectionner le coup à jouer
void bouton_gauche()
{
	//printf("Appui sur le bouton gauche\n");
	while(!digitalRead(LEFT_BUTTON_PORT));	//on attend que le bouton soit relaché
	if(colonne_choisie&0x80){					//première colonne 0b10000000
		colonne_choisie=0x02;
	}
	else{												//toute autre colonne
		colonne_choisie<<=1;
	}
	write_MAX7219(0x01, colonne_choisie);
	//printf("%x\n",colonne_choisie&0xff );
}

//fonction appellée lors de l'appel au bouton droit, "déplace" la LED de la première ligne vers la droite, ce qui permet de selectionner le coup à jouer
void bouton_droit()
{
	//printf("Appui sur le bouton droit\n");
	while(!digitalRead(RIGHT_BUTTON_PORT));//on attend que le bouton soit relaché
	if(colonne_choisie&0x02){					//dernière colonne (possible pour le puissance 4) 0b00000010
		colonne_choisie=0x80;
	}
	else{												//toute autre colonne
		colonne_choisie>>=1;
	}
	write_MAX7219(0x01, colonne_choisie);
	//printf("%x\n",colonne_choisie&0xff );
}

void animation_grille_LED(char grille[7][6], int coup)
{
	unsigned char data=0x00;					//ligne LED
	unsigned char colonne;

	switch(coup){
		case 1:
			colonne=0x80;
			break;

		case 2:
			colonne=0x40;
			break;

		case 3:
			colonne=0x20;
			break;

		case 4:
			colonne=0x10;
			break;

		case 5:
			colonne=0x08;
			break;

		case 6:
			colonne=0x04;
			break;

		case 7:
			colonne=0x02;
			break;

		default:
			break;
	}

	for(int j=0;j<6;j++){
		for(int i=0;i<7;i++){
			data<<=1;								//décale de 1 bit vers la gauche
			if(grille[i][j]!=' ') data+=1;
		}
		data<<=1;									//permet d'afficher la grille de puissance 4 dans le coin gauche de la matrice LED
		write_MAX7219((unsigned char) (j+3), data|colonne);
		delay(200);
		data=0x00;
	}

	write_MAX7219(0x01, colonne_choisie);
}