#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define P4_COLONNES (7)
#define P4_LIGNES (6)

#define J1_JETON ('O')
#define J2_JETON ('X')

#define ACT_ERR (0)
#define ACT_JOUER (1)
#define ACT_NOUVELLE_SAISIE (2)
#define ACT_QUITTER (3)

#define STATUT_OK (0)
#define STATUT_GAGNE (1)
#define STATUT_EGALITE (2)

struct position
{
    int colonne;
    int ligne;
};

void afficher_grille(char grille[7][6]);
void calcule_position(int coup, struct position *pos);
unsigned calcule_nb_jetons_depuis_vers(struct position *pos, int dpl_hrz, int dpl_vrt, char jeton);
unsigned calcule_nb_jetons_depuis(struct position *pos, char jeton);
int coup_valide(int col);
int demande_nb_joueur(void);
int grille_complete(void);
int ia(void);
void initialise_grille(void);
unsigned umax(unsigned a, unsigned b);
double nb_aleatoire(void);
int nb_aleatoire_entre(int min, int max);
int position_valide(struct position *pos);
int statut_jeu(struct position *pos, char jeton);
int vider_tampon(FILE *fp);

char grille_jeu[P4_COLONNES][P4_LIGNES];
char grille_J1[7][6];//grille correspondant au joueur 1