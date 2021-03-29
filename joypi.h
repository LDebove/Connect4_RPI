#include <stdio.h>
#include <string.h>
#include <wiringPi.h>

//MAX7219 MATRIX REGISTERS
#define DECODE_MODE 0x09
#define INTENSITY 0x0a
#define SCAN_LIMIT 0x0b 	//inutile pour notre utilisation
#define SHUTDOWN 0x0c
#define DISPLAY_TEST 0x0f


//PORTS SPI (utiles à la matrice LED) (ports WiringPi)
#define MOSI_PORT 12 		//port 19 GPIO (SPI, MOSI)
//#define MISO 13 			//port 21 GPIO (SPI, MISO) on a besoin que du MOSI
#define CLK_PORT 14 			//port 23 GPIO (SPI, CLK)
#define LOAD_PORT 11 		//port 26 GPIO (SPI, CS)

//(Physical pins)(GPIO33 Lower Button 23WPi) (GPIO22 Left button 6WPi) (GPIO35 Right button 24WPi) (GPIO37 Upper button 25WPi)
#define LOWER_BUTTON_PORT 23
#define LEFT_BUTTON_PORT 6
#define RIGHT_BUTTON_PORT 24
#define UPPER_BUTTON_PORT 25

void write_MAX7219(unsigned char regAddr, unsigned char regData);
void dessiner_grille_LED(char grille[7][6]);
void clear_LED();
void separer_grille(char grille[7][6], char grille_J1[7][6]);
void signalhandler(int signal_number);
void bouton_haut();
void bouton_bas();
void bouton_gauche();
void bouton_droit();
void init_gpio();
void init_MAX7219();
void animation_grille_LED(char grille[7][6], int coup);
