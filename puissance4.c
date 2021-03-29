#include "puissance4.h"

//permet d'affiche la grille de puissance 4 dans un terminal
void afficher_grille(char grille[7][6])
{
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][0], grille[1][0], grille[2][0], grille[3][0], grille[4][0], grille[5][0], grille[6][0]);
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][1], grille[1][1], grille[2][1], grille[3][1], grille[4][1], grille[5][1], grille[6][1]);
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][2], grille[1][2], grille[2][2], grille[3][2], grille[4][2], grille[5][2], grille[6][2]);
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][3], grille[1][3], grille[2][3], grille[3][3], grille[4][3], grille[5][3], grille[6][3]);
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][4], grille[1][4], grille[2][4], grille[3][4], grille[4][4], grille[5][4], grille[6][4]);
    printf("+---+---+---+---+---+---+---+\n");
    printf("| %c | %c | %c | %c | %c | %c | %c |\n", grille[0][5], grille[1][5], grille[2][5], grille[3][5], grille[4][5], grille[5][5], grille[6][5]);
    printf("+---+---+---+---+---+---+---+\n");
}


void calcule_position(int coup, struct position *pos)
{
    /*
     * Traduit le coup joué en un numéro de colonne et de ligne.
     */

    int lgn;

    pos->colonne = coup;

    for (lgn = P4_LIGNES - 1; lgn >= 0; --lgn)
        if (grille_jeu[pos->colonne][lgn] == ' ')
        {
            pos->ligne = lgn;
            break;
        }
}


unsigned calcule_nb_jetons_depuis_vers(struct position *pos, int dpl_hrz, int dpl_vrt, char jeton)
{
    /*
     * Calcule le nombre de jetons adajcents identiques depuis une position donnée en se
     * déplaçant de `dpl_hrz` horizontalement et `dpl_vrt` verticalement.
     * La fonction s'arrête si un jeton différent ou une case vide est rencontrée ou si
     * les limites de la grille sont atteintes.
     */

    struct position tmp;
    unsigned nb = 1;

    tmp.colonne = pos->colonne + dpl_hrz;
    tmp.ligne = pos->ligne + dpl_vrt;

    while (position_valide(&tmp))
    {
        if (grille_jeu[tmp.colonne][tmp.ligne] == jeton)
            ++nb;
        else
            break;

        tmp.colonne += dpl_hrz;
        tmp.ligne += dpl_vrt;
    }

    return nb;
}


unsigned calcule_nb_jetons_depuis(struct position *pos, char jeton)
{
    /*
     * Calcule le nombre de jetons adjacents en vérifant la colonne courante,
     * de la ligne courante et des deux obliques courantes.
     * Pour ce faire, la fonction calcule_nb_jeton_depuis_vers() est appelé à
     * plusieurs reprises afin de parcourir la grille suivant la vérification
     * à effectuer.
     */

    unsigned max;

    max = calcule_nb_jetons_depuis_vers(pos, 0, 1, jeton);
    max = umax(max, calcule_nb_jetons_depuis_vers(pos, 1, 0, jeton) + \
    calcule_nb_jetons_depuis_vers(pos, -1, 0, jeton) - 1);
    max = umax(max, calcule_nb_jetons_depuis_vers(pos, 1, 1, jeton) + \
    calcule_nb_jetons_depuis_vers(pos, -1, -1, jeton) - 1);
    max = umax(max, calcule_nb_jetons_depuis_vers(pos, 1, -1, jeton) + \
    calcule_nb_jetons_depuis_vers(pos, -1, 1, jeton) - 1);

    return max;
}


int coup_valide(int col)
{
    /*
     * Si la colonne renseignée est inférieure ou égale à zéro
     * ou que celle-ci est supérieure à la longueur du tableau
     * ou que la colonne indiquée est saturée
     * alors le coup est invalide.
     */

    if (col <= 0 || col > P4_COLONNES || grille_jeu[col - 1][0] != ' ')
        return 0;

    return 1;
}


int demande_nb_joueur(void)
{
    /*
     * Demande et récupère le nombre de joueurs.
     */

    int njoueur = 0;

    while (1)
    {
        printf("Combien de joueurs prennent part à cette partie ? \n");
        fflush(stdin);
        if (scanf("%d", &njoueur) != 1 && ferror(stdin))
        {
                fprintf(stderr, "Erreur lors de la saisie\n");
                return 0;
        }
        else if (njoueur != 1 && njoueur != 2)
            fprintf(stderr, "Plait-il ?\n");
        else
            break;

        if (!vider_tampon(stdin))
        {
            fprintf(stderr, "Erreur lors de la vidange du tampon.\n");
            return 0;
        }
    }

    return njoueur;
}


int grille_complete(void)
{
    /*
     * Détermine si la grille de jeu est complète.
     */

    unsigned col;
    unsigned lgn;

    for (col = 0; col < P4_COLONNES; ++col)
        for (lgn = 0; lgn < P4_LIGNES; ++lgn)
            if (grille_jeu[col][lgn] == ' ')
                return 0;

    return 1;
}


int ia(void)
{
    /*
     * Fonction mettant en œuvre l'IA présentée.
     * Assigne une valeur pour chaque colonne libre et retourne ensuite le numéro de
     * colonne ayant la plus haute valeur. Dans le cas où plusieurs valeurs égales sont
     * générées, un numéro de colonne est « choisi au hasard » parmi celles-ci.
     */

    unsigned meilleurs_col[P4_COLONNES];
    unsigned nb_meilleurs_col = 0;
    unsigned max = 0;
    unsigned col;

    for (col = 0; col < P4_COLONNES; ++col)
    {
        struct position pos;
        unsigned longueur;

        if (grille_jeu[col][0] != ' ')
            continue;

        calcule_position(col, &pos);
        longueur = calcule_nb_jetons_depuis(&pos, J2_JETON);

        if (longueur >= 4)
            return col;

        longueur = umax(longueur, calcule_nb_jetons_depuis(&pos, J1_JETON));

        if (longueur >= max)
        {
            if (longueur > max)
            {
                nb_meilleurs_col = 0;
                max = longueur;
            }

            meilleurs_col[nb_meilleurs_col++] = col;
        }
    }

    return meilleurs_col[nb_aleatoire_entre(0, nb_meilleurs_col - 1)];
}


void initialise_grille(void)
{
    /*
     * Initalise les caractères de la grille.
     */

    unsigned col;
    unsigned lgn;

    for (col = 0; col < P4_COLONNES; ++col)
        for (lgn = 0; lgn < P4_LIGNES; ++lgn){
            grille_jeu[col][lgn] = ' ';
            grille_J1[col][lgn] = ' ';
        }
}


unsigned umax(unsigned a, unsigned b)
{
    /*
     * Retourne le plus grand des deux arguments.
     */

    return (a > b) ? a : b;
}


double nb_aleatoire(void)
{
    /*
     * Retourne un nombre pseudo-aléatoire compris entre zéro inclus et un exclus.
     */

    return rand() / ((double)RAND_MAX + 1.);
}


int nb_aleatoire_entre(int min, int max)
{
    /*
     * Retourne un nombre pseudo-aléatoire entre `min` et `max` inclus.
     */

    return nb_aleatoire() * (max - min + 1) + min;
}


int position_valide(struct position *pos)
{
    /*
     * Vérifie que la position fournie est bien comprise dans la grille.
     */

    int ret = 1;

    if (pos->colonne >= P4_COLONNES || pos->colonne < 0)
        ret = 0;
    else if (pos->ligne >= P4_LIGNES || pos->ligne < 0)
        ret = 0;

    return ret;
}


int statut_jeu(struct position *pos, char jeton)
{
    /*
     * Détermine s'il y a lieu de continuer le jeu ou s'il doit être
     * arrêté parce qu'un joueur a gagné ou que la grille est complète.
     */

    if (grille_complete())
        return STATUT_EGALITE;
    else if (calcule_nb_jetons_depuis(pos, jeton) >= 4)
        return STATUT_GAGNE;

    return STATUT_OK;
}


int vider_tampon(FILE *fp)
{
    /*
     * Vide les données en attente de lecture du flux spécifié.
     */

    int c;

    do
        c = fgetc(fp);
    while (c != '\n' && c != EOF);

    return ferror(fp) ? 0 : 1;
}