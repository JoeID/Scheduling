/***
Copyright (c) 2018 Guiraud Maël
All rights reserved.
*///

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "scheduling_mael.h"
#include "const.h"

//#define DEBUG

Ensemble *crisis(Ensemble *ens, Route *crisise, Route *elemspere,
                 Route *touselems, int taille_paquet, int periode);
Ensemble *init_ensemble() { return NULL; }
Route *init_element() { return NULL; }

void decaler_release(int *release, int *deadline, int periode, int premier,
                     int nbr_route, int taille_paquet);

/** Fonctions d'affichage pour simons *


*****************/
int mod(int a, int b)
{
    if (a >= 0)
        return a % b;
    else {
        while (a < 0) { // WARNING pk coder le modulo de cette manière ?

            a += b;
        }
        return a % b;
    }
}
int mod2(int a, int b) { return (a % b >= 0) ? a % b : a % b + b; } // mieux

void affiche_ensemble_unique(Ensemble *ens)
{
    if (!ens) {
        printf("VIDE\n");
        return;
    }
    printf("(%p) num %d / date %d /fils g %p /frereG %p /frereD %p\n", ens,
           ens->numero_element, ens->temps_depart, ens->filsG, ens->frereG,
           ens->frereD);
}

void affiche_ensemble(Ensemble *ens)
{
    // affiche_1_ensemble(ens);
    if (ens == NULL) {
        printf("ENSEMBLE VIDE");
        return;
    }
    if (ens->numero_element == -1) // si c'est un ensemble
    {
        // si c'est un ensemble, par définition il a forcement un fils gauche
        // qui est soit un élément, soit un ensemble, donc appel recursif sur le
        // fils gauche
        printf("Sous ensemble(%d){ ", ens->temps_depart);
        affiche_ensemble(ens->filsG);
        printf("} ");
    }
    else // si c'est un élément
    {
        printf("%d(%d) ", ens->numero_element, ens->temps_depart);
    }
    ens = ens->frereD;
    while (ens) {
        if (ens->numero_element == -1) {
            affiche_ensemble(ens);
            return;
        }
        else {
            printf("%d(%d) ", ens->numero_element, ens->temps_depart);
        }
        ens = ens->frereD;
    }
}

Route *ajoute_elemt(Route *elem, int index, int release, int deadline)
{
    Route *tmp = elem;
    Route *new = malloc(sizeof(Route));
    new->index = index;
    new->release = release;
    new->deadline = deadline;
    new->next = NULL;
    if (elem == NULL) // 0 element
    {
        return new;
    }

    // test pour l'element 1
    if (elem->release > release) // inser avant
    {
        new->next = elem;
        return new;
    }

    while (elem->next) { 

        if (elem->next->release > release) // inser avant
        {
            new->next = elem->next;
            elem->next = new;
            return tmp;
        }
        elem = elem->next;
    }
    elem->next = new;
    return tmp;
}

Route *ajoute_elemt_fin(Route *elem, int index, int release, int deadline)
{
    Route *tmp = elem;
    Route *new = malloc(sizeof(Route));
    new->index = index;
    new->release = release;
    new->deadline = deadline;
    new->next = NULL;
    if (elem == NULL) // 0 element
    {
        return new;
    }
    while (elem->next) {

        elem = elem->next;
    }
    elem->next = new;
    return tmp;
}

Ensemble *cree_ensemble(int numero, int temps)
{
    Ensemble *ens = malloc(sizeof(Ensemble));
    ens->numero_element = numero;
    ens->temps_depart = temps;
    ens->filsG = NULL;
    ens->frereG = NULL;
    ens->frereD = NULL;
    return ens;
}

int compte_elems(Route *l)
{
    if (l == NULL)
        return 0;
    int compteur = 1;
    while (l->next) {
        compteur++;
        l = l->next;
    }
    return compteur;
}

void recalculer_deadlines(Route *elems, int deadline_periode)
{
    while (elems) {
        if (elems->deadline > deadline_periode)
            elems->deadline = deadline_periode;
        elems = elems->next;
    }
}
Route *tri_elems(Route *l)
{
    int taille = compte_elems(l);
    if (taille < 2)
        return l;
    int i, j;
    Route *debut = l;
    Route *tmp;
    for (i = taille - 1; i > 0; i--) {
        l = debut;
        if (l->release > l->next->release) {
            tmp = l->next;
            l->next = l->next->next;
            tmp->next = l;
            debut = tmp;
            l = debut;
        }
        for (j = 0; j < i - 1; j++) {
            if (l->next->release > l->next->next->release) {
                tmp = l->next;
                l->next = l->next->next;
                tmp->next = l->next->next;
                l->next->next = tmp;
            }
            l = l->next;
        }
    }
    return debut;
}

Route *retire_element_i(Route *elem, int index)
{
    if (elem == NULL) {
        printf("aucun element");
        return NULL;
    }
    Route *tmp;
    Route *debut = elem;
    if (elem->index == index) // cas du premier element
    {
        tmp = elem;
        debut = elem->next;
        if (tmp)
            free(tmp);
        return debut;
    }
    while (elem->next) {
        if (elem->next->index == index) {
            tmp = elem->next;
            elem->next = elem->next->next;
            if (tmp)
                free(tmp);
            return debut;
        }
        elem = elem->next;
    }
    return debut;
}
Route *get_element_i(Route *elem, int index)
{
    while (elem) {
        if (elem->index == index) {
            // printf("%d %d\n",elem->index,index);
            return elem;
        }
        elem = elem->next;
    }
    return NULL;
}
void freeelems(Route *elem)
{
    Route *tmp;
    while (elem) {
        tmp = elem;
        elem = elem->next;
        if (tmp)
            free(tmp);
    }
}
void affichejobs(Route *elem)
{
    if (!elem)
        printf("aucun element");
    // printf("Routes\n");
    while (elem) {
        printf("%d(%d,%d),  ", elem->index, elem->release, elem->deadline);
        elem = elem->next;
    }
    printf("\n");
}
Route *cpy_elems(Route *elems)
{
    Route *new = init_element();
    while (elems) {
        new = ajoute_elemt(new, elems->index, elems->release, elems->deadline);
        elems = elems->next;
    }
    return new;
}
int date_fin(Ensemble *ens, int taille_paquet)
{
    while (ens->frereD) {
        ens = ens->frereD;
    }

    // affiche_ensemble_unique(ens);
    if (ens->numero_element == -1) {
        return date_fin(ens->filsG, taille_paquet);
    }
    else {
        return ens->temps_depart + taille_paquet;
    }
}
int date_debut(Ensemble *ens)
{
    if (ens->numero_element == -1) {
        return date_debut(ens->filsG);
    }
    else {
        return ens->temps_depart;
    }
}

// renvoie l'element eligible
int eligible(Route *elems, int time)
{

    Route *debut = elems;
    if (elems == NULL)
        return -1;
    // printf("pointeur envoyé = %p\n",elems);
    int deadline_min = INT_MAX;
    int elu = -1;
    while (elems) {
        if (elems->release <= time) // si dispo
        {
            if (elems->deadline < deadline_min) {
                deadline_min = elems->deadline;
                elu = elems->index;
            }
        }
        elems = elems->next;
    }
    if (elu != -1) {

        return elu;
    }
    else {
        // Ici, on saute dans la backward periode, on doit regarder si le
        // premier message est le seul a revenir a sa date
        elems = debut;
        Route *retour = elems;
        while (elems) {
            if (elems->release > retour->release) {
                break;
            }
            // sinon, c'est =, on compare donc les deadlines
            if (elems->deadline < retour->deadline) {
                retour = elems;
            }
            elems = elems->next;
        }

        return retour->index; // la liste est triée par release time, donc si on
                              // ne trouve pas d'eligible, on saute direct au
                              // premier release
    }
}
void libereens(Ensemble *ens)
{

    // printf("entree dans la fonction libere\n");
    if (ens) {
        if (ens->numero_element == -1) {
            // printf("On libere l'ensemble a la date %d",ens->temps_depart);
            libereens(ens->filsG);
        }
        // printf("On libere l'ensemble a droite de %d \n",ens->numero_element
        // );
        libereens(ens->frereD);
        free(ens);
        ens = NULL;
    }
}
Ensemble *cpyens(Ensemble *ens)
{
    if (ens == NULL)
        return NULL;
    Ensemble *cpy = NULL;
    cpy = cree_ensemble(ens->numero_element, ens->temps_depart);
    if (ens->numero_element == -1) // si c'est un ensemble
    {
        cpy->filsG = cpyens(ens->filsG);
    }
    cpy->frereD = cpyens(ens->frereD);
    if (cpy->frereD)
        cpy->frereD->frereG = cpy;
    return cpy;
}
Ensemble *invade(Ensemble *ens, Route *touselems, int depart,
                 int taille_paquet, int periode)
{

    Ensemble *ens2 = NULL;
    Ensemble *tmp = NULL;

    // printf("ENTREE DANS LA FONCTION INVADE date
    // %d-------------------------------\n",depart);
    // affiche_ensemble(ens);printf("\n");
    // on se place a droite de l'ensemble qui a crisis
    while (ens->frereD) {
        ens = ens->frereD;
    }
    Route *elems = init_element();
    Route *elemtmp = NULL;
    int date = depart;
    // on backtrack jusqu'a la fin
    while (ens) {
        if (ens->numero_element != -1) // si c'est un element
        {
            elemtmp = get_element_i(touselems, ens->numero_element);
            if (ens2 == NULL) // initialisation
            {
                ens2 = cree_ensemble(-2, -1);
            }
            else {
                tmp = ens2;
                ens2->frereG = cree_ensemble(-2, -1);
                ens2 = ens2->frereG;
                ens2->frereD = tmp;
            }
            elems = ajoute_elemt(elems, elemtmp->index, elemtmp->release,
                                 elemtmp->deadline);
            // printf("Ajout de %d dans elems \n",elemtmp->index);
            // affichejobs(elems);
        }

        else // si c'est un ensemble
        {
            if (ens2 == NULL) {
                ens2 = cpyens(ens);
            }
            else {
                ens2->frereG = cpyens(ens);
                ens2->frereG->frereD = ens2;
                ens2 = ens2->frereG;
            }
        }
        if (ens->frereG) // on regarde si on est pas au debut de l'ensemble
        {

            // printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
            ens = ens->frereG;
            libereens(ens->frereD);
            ens->frereD = NULL;
        }
        else {
            // printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
            libereens(ens);
            ens = NULL;
        }
    }
    // affiche_ensemble(ens2);printf(" R.S.\n");
    // affichejobs(elems);

    Ensemble *ens3 = NULL;
    Ensemble *ens4 = NULL;
    int i;

    while (ens2) // parcours de ens2
    {
        if (ens2->numero_element == -2) // si on attend un element
        {

            i = eligible(elems, date);
            if (i == -1) {
                // printf("Failure 1\n");
                libereens(ens2);

                libereens(ens4);
                freeelems(elems);
                return NULL;
            }
            elemtmp = get_element_i(touselems, i);
            if (elemtmp == NULL) {
                // printf("Failure 0\n");
                libereens(ens2);
                libereens(ens4);
                freeelems(elems);
                return NULL;
            }

            date = max(date, elemtmp->release);
            if (date + taille_paquet > elemtmp->deadline) // CRISIS
            {
                // printf("Crisis(invade) sur la tache %d\n",elemtmp->index);
                Route *crisisrec = ajoute_elemt(
                    NULL, elemtmp->index, elemtmp->release, elemtmp->deadline);
                elems = retire_element_i(elems, crisisrec->index);
                if (!elems)
                    elems = ajoute_elemt(elems, 99999, 99999, 999999);
                ens3 = crisis(ens3, crisisrec, elems, touselems, taille_paquet,
                              periode);
                freeelems(crisisrec);
                // printf("-------------\n\n\n");
                // affichejobs(elems);
                // affiche_ensemble(ens2);		printf("\n");
                // affiche_ensemble(ens3);
                // printf("-------------\n\n\n");
                if (ens3 == NULL) {
                    // printf(" NO PULL(%d) FOUND(invade)\n",elemtmp->index);
                    libereens(ens2);
                    freeelems(elems);
                    return NULL;
                }

                while (ens3->frereD)
                    ens3 = ens3->frereD;
                // affiche_ensemble(ensembletmp);printf("Elemenent le plus a
                // droite\n");
                date = date_fin(ens3, taille_paquet);

                elems = tri_elems(elems);

                // rajouter un element pour rescheduler PULL(X) apres la crisis
                // dans l'invasion
                if (ens2 == NULL) // initialisation
                {
                    ens2 = cree_ensemble(-2, -1);
                }
                else {
                    tmp = ens2;
                    ens2->frereG = cree_ensemble(-2, -1);
                    ens2 = ens2->frereG;
                    ens2->frereD = tmp;
                }
            }
            else // PAS DE CRISE
            {

                if (ens3 == NULL) // premier element de ens3
                {
                    ens3 = cree_ensemble(elemtmp->index, date);
                    ens4 = ens3;
                }
                else {

                    ens3->frereD = cree_ensemble(elemtmp->index, date);
                    ens3->frereD->frereG = ens3;
                    ens3 = ens3->frereD;
                }
                elems = retire_element_i(elems, elemtmp->index);
                // printf("on Schedule %d a la date %d et on le retire de
                // elem",elemtmp->index,date);
                date += taille_paquet;

                if (ens2->frereD) // Si il y a un prochain element
                {
                    if (ens2->frereD->numero_element ==
                        -1) // et que c'est un ensemble
                    {
                        if (date + taille_paquet >=
                            ens2->frereD->temps_depart) // ET que ca INVADE
                        {
                            // printf("%d INVADE date
                            // %d\n",elemtmp->index,date);
                            ens2->frereD->filsG = invade(
                                ens2->frereD->filsG, touselems,
                                date + taille_paquet, taille_paquet, periode);
                            if (ens2->frereD->filsG)
                                ens2->frereD->temps_depart =
                                    ens2->frereD->filsG->temps_depart;
                            else {
                                libereens(ens2);
                                libereens(ens4);
                                freeelems(elems);

                                return NULL;
                            }
                        }
                    }
                }
            }
            // affiche_ensemble(ens3);printf(" Route\n");
        }
        else // si c'est un ensemble
        {
            if (date + taille_paquet >= ens2->temps_depart) // INVADE Si besoin
            {
                // j'ai modifié ici

                ens2->filsG =
                    invade(ens2->filsG, touselems, date + taille_paquet,
                           taille_paquet, periode);
                if (ens2->filsG)
                    ens2->temps_depart = ens2->filsG->temps_depart;
                else {
                    libereens(ens2);
                    libereens(ens4);
                    freeelems(elems);

                    return NULL;
                }
            }

            // On recopie ens2 dans ens3

            if (ens3 == NULL) {

                ens3 = cree_ensemble(-1, ens2->temps_depart);
                ens3->filsG = cpyens(ens2->filsG);
                ens4 = ens3;
            }
            else {
                ens3->frereD = cree_ensemble(-1, ens2->temps_depart);
                ens3->frereD->filsG = cpyens(ens2->filsG);
                ens3->frereD->frereG = ens3;
                ens3 = ens3->frereD;
            }
            // affiche_ensemble(ens2);
            date = date_fin(ens3, taille_paquet); // printf("fin = %d \n",date);
        }
        tmp = ens2;
        ens2 = ens2->frereD;
        if (ens2 != NULL)
            ens2->frereG = NULL;
        if (tmp)
            free(tmp);
    }

    // on remet ens3 a son debut

    while (ens3->frereG)
        ens3 = ens3->frereG;
    // affiche_ensemble(ens3);printf("RETOUR INVADE --------------------\n");
    return ens3;
}
Ensemble *crisis(Ensemble *ens, Route *crisise, Route *elemspere,
                 Route *touselems, int taille_paquet, int periode)
{
    // printf("------------------ENTREE DANS LA FONCTION
    // CRISIS-----------------------------\n");
    if (ens == NULL)
        return NULL;
    // printf("ens != NULL\n");
    // printf("Ensemble et elems pere a l'entree de crisis\n");
    // affiche_ensemble(ens);printf("\n");
    // affichejobs(elemspere);
    // printf("\n Et la crisis est :");affichejobs(crisise);printf("\n");
    // printf("fin\n");
    Ensemble *ens2 = NULL;
    Ensemble *tmp = NULL;
    Ensemble *debut = ens;
    // on se place a droite de l'ensemble qui a crisis
    while (ens->frereD) {
        ens = ens->frereD;
    }
    Route *elems = init_element();
    Route *elemtmp = NULL;
    int date = 0;
    elems = ajoute_elemt(elems, crisise->index, crisise->release,
                         crisise->deadline);
    // on backtrack jusqu'a PULL de X ou a la fin
    while (ens) {

        if (ens->numero_element != -1) // si c'est un element
        {
            elemtmp = get_element_i(touselems, ens->numero_element);
            // printf("Elemtmp = %d\n",elemtmp->index);
            if (elemtmp->deadline > crisise->deadline) // si on est sur PULL(X)
            {
                // printf("PULL X = %d\n",elemtmp->index);
                elemspere =
                    ajoute_elemt_fin(elemspere, elemtmp->index,
                                     elemtmp->release, elemtmp->deadline);
                date = ens->temps_depart;
                // printf("PULL(%d) = %d\n",crisise->index,elemtmp->index);
                if (ens2 == NULL) {
                    ens2 = cree_ensemble(-2, -1);
                }
                else {
                    tmp = ens2;
                    ens2->frereG = cree_ensemble(-2, -1);
                    ens2 = ens2->frereG;
                    ens2->frereD = tmp;
                }
                if (ens->frereG) // on regarde si on est pas au debut de
                                 // l'ensemble
                {
                    ens = ens->frereG;
                    libereens(ens->frereD);
                    ens->frereD = NULL;
                }
                else {
                    libereens(ens);
                    ens = NULL;
                    debut = NULL;
                }
                break;
            }
            if (ens2 == NULL) // initialisation
            {
                ens2 = cree_ensemble(-2, -1);
            }
            else {
                tmp = ens2;
                ens2->frereG = cree_ensemble(-2, -1);
                ens2 = ens2->frereG;
                ens2->frereD = tmp;
            }

            elems = ajoute_elemt(elems, elemtmp->index, elemtmp->release,
                                 elemtmp->deadline);
        }

        else // si c'est un ensemble
        {
            if (ens2 == NULL) {
                ens2 = cpyens(ens);
            }
            else {
                ens2->frereG = cpyens(ens);
                ens2->frereG->frereD = ens2;
                ens2 = ens2->frereG;
            }
        }

        if (ens->frereG) // on regarde si on est pas au debut de l'ensemble
        {

            // printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
            ens = ens->frereG;
            libereens(ens->frereD);
            ens->frereD = NULL;
        }
        else {
            // printf("On ajoute ");affiche_ensemble(ens);printf("au r.s\n");
            libereens(ens);
            ens = NULL;
            debut = NULL;
        }

        // si ens est null alors on n'a pas trouvé PULL(X) (si on a trouvé pull
        // X on break)
        if (ens == NULL)

        {
            libereens(ens2);
            freeelems(elems);
            return NULL;
        }
    }

    Ensemble *ens3 = NULL;
    Ensemble *ens4 = NULL;
    int i;
    // printf("avant le rescheduling date = %d\n",date);
    // affiche_ensemble(ens2);
    // affichejobs(elems);
    // printf("\n");
    while (ens2) // parcours de ens2
    {
        if (ens2->numero_element == -2) // si on attend un element
        {

            i = eligible(elems, date);
            // printf("I = %d \n",i);
            if (i == -1) {
                // printf("Failure 1\n");
                libereens(ens2);
                libereens(debut);
                freeelems(elems);
                libereens(ens4);
                return NULL;
            }
            elemtmp = get_element_i(touselems, i);
            if (elemtmp == NULL) {
                // printf("Failure 0\n");
                libereens(ens2);
                libereens(debut);
                freeelems(elems);
                libereens(ens4);
                return NULL;
            }

            date = max(date, elemtmp->release);
            if (date + taille_paquet > elemtmp->deadline) // CRISIS
            {

                // printf("crisis(crisis) sur la tache %d\n",elemtmp->index);
                Route *crisisrec = ajoute_elemt(
                    NULL, elemtmp->index, elemtmp->release, elemtmp->deadline);
                elems = retire_element_i(elems, crisisrec->index);
                if (!elems)
                    elems = ajoute_elemt(elems, INT_MAX, INT_MAX, INT_MAX);
                // printf("Jobs avant crisis\n");affichejobs(elems);printf("
                // \n");
                ens4 = crisis(ens4, crisisrec, elems, touselems, taille_paquet,
                              periode);
                freeelems(crisisrec);
                // printf("Ensemble apres
                // crisis\n");affiche_ensemble(ens4);printf(" \n"); printf("jobs
                // apres crisis\n");affichejobs(elems);printf(" \n");
                if (ens4 == NULL) {
                    // printf(" NO PULL(%d) FOUND\n",elemtmp->index);
                    libereens(ens2);
                    libereens(debut);
                    libereens(ens4);
                    freeelems(elems);
                    return NULL;
                }
                ens3 = ens4;
                while (ens3->frereD)
                    ens3 = ens3->frereD;
                // affiche_ensemble(ensembletmp);printf("Elemenent le plus a
                // droite\n");
                date = date_fin(ens3, taille_paquet);
                // recalculer_deadlines(elems,ens4->temps_depart+periode);
                elems = tri_elems(elems);
                // ajoute d'un elem -2 a ens 2

                Ensemble *temporaire = ens2->frereD;
                ens2->frereD = cree_ensemble(-2, -1);
                ens2->frereD->frereG = ens2;
                ens2->frereD->frereD = temporaire;
            }

            else // PAS DE CRISE
            {

                if (ens3 == NULL) // premier element de ens3
                {
                    ens3 = cree_ensemble(elemtmp->index, date);
                    ens4 = ens3;
                }
                else {
                    ens3->frereD = cree_ensemble(elemtmp->index, date);
                    ens3->frereD->frereG = ens3;
                    ens3 = ens3->frereD;
                }
                elems = retire_element_i(elems, elemtmp->index);
                date += taille_paquet;
                if (ens2->frereD) // Si il y a un prochain element
                {
                    if (ens2->frereD->numero_element ==
                        -1) // et que c'est un ensemble
                    {
                        if (date >=
                            ens2->frereD->temps_depart) // ET que ca INVADE
                        {
                            // printf("%d INVADE date
                            // %d\n",elemtmp->index,date);

                            ens2->frereD->filsG =
                                invade(ens2->frereD->filsG, touselems, date,
                                       taille_paquet, periode);
                            if (ens2->frereD->filsG)
                                ens2->frereD->temps_depart =
                                    ens2->frereD->filsG->temps_depart;
                            else {
                                libereens(ens2);
                                libereens(ens4);
                                libereens(debut);
                                freeelems(elems);
                                return NULL;
                            }
                        }
                    }
                }
            }

            // affiche_ensemble(ens3);printf(" Route\n");
        }
        else // si c'est un ensemble
        {

            if (ens3 == NULL) {

                ens3 = cree_ensemble(-1, ens2->temps_depart);
                ens3->filsG = cpyens(ens2->filsG);
                ens4 = ens3;
            }
            else {
                ens3->frereD = cree_ensemble(-1, ens2->temps_depart);
                ens3->frereD->filsG = cpyens(ens2->filsG);
                ens3->frereD->frereG = ens3;
                ens3 = ens3->frereD;
            }
            // affiche_ensemble(ens2);
            date = date_fin(ens3, taille_paquet); // printf("fin = %d \n",date);
            if (ens2->frereD) // Si il y a un prochain element
            {
                if (ens2->frereD->numero_element ==
                    -1) // et que c'est un ensemble
                {
                    if (date >= ens2->frereD->temps_depart) // ET que ca INVADE
                    {
                        // printf("%d INVADE date %d\n",elemtmp->index,date);

                        ens2->frereD->filsG =
                            invade(ens2->frereD->filsG, touselems, date,
                                   taille_paquet, periode);
                        if (ens2->frereD->filsG)
                            ens2->frereD->temps_depart =
                                ens2->frereD->filsG->temps_depart;
                        else {
                            libereens(ens2);
                            libereens(debut);
                            freeelems(elems);
                            libereens(ens4);
                            return NULL;
                        }
                    }
                }
            }
        }

        tmp = ens2;
        ens2 = ens2->frereD;
        if (ens2 != NULL)
            ens2->frereG = NULL;
        if (tmp)
            free(tmp);
    }
    ens3 = ens4;

    // on remet ens3 a son debut
    // affiche_ensemble(ens);printf(" -----------------------------\n");
    while (ens3->frereG)
        ens3 = ens3->frereG;
    Ensemble *newfils = cree_ensemble(-1, ens3->temps_depart);
    newfils->filsG = ens3;

    freeelems(elems);
    if (!ens) // si ens est vide, alors on est au tout debut
    {
        // printf("RETOUR DE CRISIS--------------------------------------");
        // affiche_ensemble(newfils);printf("\n");
        return newfils;
    }
    ens->frereD = newfils;
    newfils->frereG = ens;
    // on donne a ens un element avec ens3 comme filsgauche
    // printf("RETOUR DE CRISIS--------------------------------------");
    // affiche_ensemble(debut);printf("\n");

    return debut;
}

void transforme_waiting(Ensemble *ens, int *wi)
{

    if (ens) {
        if (ens->numero_element < 0) {
            transforme_waiting(ens->filsG, wi);
            transforme_waiting(ens->frereD, wi);
        }
        else {
            wi[ens->numero_element] = ens->temps_depart;
            transforme_waiting(ens->frereD, wi);
        }
    }
}

/*
int* retourne_offset(Graphe g, int taille_paquet, int* permutation,int mode,int
periode)
{
        int nbr_route = g->N/2;
        int * offsets;
        assert(offsets=(int*)malloc(sizeof(int)*nbr_route));
        int offset = 0;
        int budget = periode - (nbr_route*taille_paquet);

        int inter_size = budget / nbr_route;
        int * random;

        switch(mode)
        {
                case 0:
                        for(int i=0;i<nbr_route;i++)
                        {
                                        offsets[permutation[i]]=offset;
                                        offset+=taille_paquet;

                        }
                break;
                case 1:
                        for(int i=0;i<nbr_route;i++)
                        {
                                        offsets[permutation[i]]=offset;
                                        offset+=taille_paquet+inter_size;


                        }
                break;
                default:
                        random = random_inters(budget, nbr_route);
                        for(int i=0;i<nbr_route;i++)
                        {

                                        offsets[permutation[i]]=offset;
                                        offset+=taille_paquet+random[i];
                        }
                        free(random);

                break;
        }

        return offsets;
}*/
/*int *retourne_departs(Graphe g, int * offsets)
{
        int nbr_route = g->N/2;
        int* m_i = (int*)malloc(sizeof(int)*nbr_route);
        int min = INT_MAX;
        for(int i=0;i<nbr_route;i++)
        {
                m_i[i] = offsets[i] - g->matrice[nbr_route][i];
                if(m_i[i]<min)
                        min = m_i[i];
        }
        for(int i=0;i<nbr_route;i++)
        {
                m_i[i] += (-min);
        }
        return m_i;
}*/
Ensemble *algo_simons(Route *elems, int nbr_route, int taille_paquet,
                      int date, int periode)
{
    int i;
    Route *elems2 = cpy_elems(elems);
    Route *tmp = elems2;

    int a_scheduler = nbr_route;
    /////////////////////////////////////////////////////int a_scheduler = 11;
    Ensemble *ens = NULL;
    Ensemble *ensembletmp = NULL;
    while (a_scheduler != 0) // tant qu'on n'a pas schedul tous les elements
    {
        // printf("test %p\n",elems2);
        i = eligible(elems2, date);
        // printf("i = %d\n",i);
        if (i == -1) {
            printf("Failure 1\n");
            return NULL;
        }
        tmp = get_element_i(elems2, i);

        if (tmp == NULL) {
            printf("Failure 0\n");
            return NULL;
        }

        date = max(date, tmp->release);
        if (date + taille_paquet > tmp->deadline) // CRISIS
        {

            // printf("Crisis(main) sur la tache %d\n",tmp->index);
            // affiche_ensemble(ens);printf(" Avant\n");
            Route *crisise = NULL;
            crisise =
                ajoute_elemt(crisise, tmp->index, tmp->release, tmp->deadline);

            elems2 = retire_element_i(elems2, crisise->index);
            if (!elems2)
                elems2 = ajoute_elemt(elems2, INT_MAX, INT_MAX, INT_MAX);
            // printf("Taches avant crisis\n");affichejobs(elems2);printf("\n");
            // printf("ens avant crisis\n");affiche_ensemble(ens);printf("\n");
            ens = crisis(ens, crisise, elems2, elems, taille_paquet, periode);
            freeelems(crisise);
            // printf("Taches apres crisis\n");affichejobs(elems2);printf("\n");
            // printf("ens apres crisis\n");affiche_ensemble(ens);printf("\n");
            // affiche_ensemble(ens);printf(" Apres\n");
            if (ens == NULL) {
                // affichejobs(elems2);
                // printf(" NO PULL(%d) FOUND\n",crisise->index);
				// WARNING ne pas free elems ici !!!!!
                freeelems(elems2);
                return NULL;
            }
            // on se remet a droite de l'ensemble
            ensembletmp = ens;
            while (ensembletmp->frereD)
                ensembletmp = ensembletmp->frereD;
            // affiche_ensemble(ensembletmp);printf("Elemenent le plus a
            // droite\n");
            date = date_fin(ens, taille_paquet);

            /*printf("APRESCRISIS\n");
            printf("Date %d : ",date);*/
            // recalculer_deadlines(elems2,ens->temps_depart+periode);
            elems2 = tri_elems(elems2);
            /*affichejobs(elems2);
            affiche_ensemble(ens);printf("\n\n\n");*/
        }
        else // pas crisis
        {
            // printf("AVANT eligible = %d\n",i);
            // printf("Date %d : ",date);
            // affichejobs(elems2);
            // affiche_ensemble(ens);printf("\n");
            if (ens == NULL) // si c'est la premiere fois
            {
                ens = cree_ensemble(tmp->index, date);
                ensembletmp = ens;
            }
            else {
                ensembletmp->frereD = cree_ensemble(tmp->index, date);
                ensembletmp->frereD->frereG = ensembletmp;
                ensembletmp = ensembletmp->frereD;
            }
            // printf("APRES\n");
            elems2 = retire_element_i(elems2, tmp->index);
            a_scheduler--;
            // printf("Date %d : ",date);
            // affichejobs(elems2);
            // affiche_ensemble(ens);printf("\n\n\n");
            date += taille_paquet;
        }
    }
    // affiche_ensemble(ens);
    // ecriture des temps trouvés

    freeelems(elems2);
    return ens;
}

/*int* normaliser_instance(Graphe g, int periode)
{
        int nbr_route = g->N/2;
        int * instance = (int *)malloc(sizeof(int)*nbr_route);
        for(int i=0;i<nbr_route;i++)
        {
                instance[i] = g->matrice[nbr_route][i+nbr_route+1]; // juste le
retour while((2 * instance[i]) >= periode)
                {
                        instance[i] -= periode/2;
                }
        }
        return instance;

}
*/

void decaler_release(int *release, int *deadline, int periode, int premier,
                     int nbr_route, int taille_paquet)
{
    int releasepremier = release[premier];

    for (int i = 0; i < nbr_route; i++) {
        // printf("Pour la route %d [%d;%d]\n",i,release[i],deadline[i]);
        release[i] -= releasepremier;
        deadline[i] -= releasepremier;
        // printf("apres : %d %d\n",release[i],deadline[i]);
        while (release[i] < 0) {
            // printf("Ajout de periode \n");
            release[i] += periode;
            deadline[i] += periode;
            // printf(" %d %d\n",release[i],deadline[i]);
        }
        // printf("Derniere modif \n");
        deadline[i] = deadline[i] - periode * (release[i] / periode);
        release[i] = mod(release[i], periode);

        // printf("apres2 : %d %d\n",release[i],deadline[i]);

        if (release[i] > (periode - taille_paquet)) {
            release[i] = 0;
            deadline[i] -= periode;
        }
    }
}
int check_Deadlines(int *tab, int size)
{
    for (int i = 0; i < size; i++) {
        if (tab[i] < 0)
            return 0;
    }
    return 1;
}

int wrapper_algo(int *release, int *deadline, int nbr_route, int job_size) // WARNING pk une liste chainee
{
    Route *elems;
    Ensemble *ens;
    elems = init_element();
    for (int j = 0; j < nbr_route; j++) {
        elems = ajoute_elemt(elems, j, release[j], deadline[j]);
    }
    // printf("FPT premier = %d\n",premier);
    printf("Les jobs sont : \n");
    affichejobs(elems);
    ens = algo_simons(elems, nbr_route, job_size, 0, 0);
    if (ens == NULL) {
        printf("Pas de solution. \n");
        return -1;
    }

    int temps = date_fin(ens, job_size) - date_debut(ens);
    printf("La solution est : \n");
    affiche_ensemble(ens);
	printf("\n");
    libereens(ens); // WARNING le franglais mdr
    freeelems(elems);
    return temps;
}

#ifdef DEBUG
struct Task{
	int release_time;
	int deadline;
};
typedef struct Task Task;

int main()
{
    srand(time(NULL)); // initialize seed
	FILE *src = fopen("sched_tests/debug", "r");
	int n, job_size = D;
	
	fscanf(src, "%d\n\n", &n);
    fscanf(src, "%d\n", &n);
	int release[n], deadline[n];
    for (int i = 0; i < n; i++) {
        fscanf(src, "%d %d\n", &release[i],
               &deadline[i]);
    }
	
    printf("Temps effectif %d \n",
           wrapper_algo(release, deadline, n, job_size));

	fclose(src);
    return 0;
}
#endif