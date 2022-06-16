#ifndef SCHED_MAEL
#define SCHED_MAEL

typedef struct element { // liste chaînée des tâches
    int index;
    int release;
    int deadline;
    struct element *next;
} Route;

typedef struct ensemble { // scheduling
    int numero_element;   // vaut -1 si c'est un ensemble
    int temps_depart;
    struct ensemble *filsG;
    struct ensemble *frereG;
    struct ensemble *frereD;
} Ensemble;

Ensemble *algo_simons(Route *elems, int nbr_route, int taille_paquet, int date,
                      int periode);
void affichejobs(Route *elem);
void freeelems(Route *elem);
void libereens(Ensemble *ens);
Route *init_element();
Route *ajoute_elemt(Route *elem, int index, int release, int deadline);
void affiche_ensemble(Ensemble *ens);

#endif