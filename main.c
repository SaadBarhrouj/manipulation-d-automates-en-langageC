
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TAILLE_ALPHABET 26
#define MAX_TRANSITIONS 100
#define MAX_ETAT 10
#define MAX_MOTS 100

// Structure de Transition pour représenter une transition entre deux états
typedef struct {
    int etat_de_depart; // État de départ de la transition
    int etat_d_arrive; // État d'arrivée de la transition
    char etiquette_de_transition; // Étiquette de la transition
} Transition;

// Structure Automate pour représenter un automate
typedef struct {
    Transition transitions[MAX_TRANSITIONS]; // Tableau de transitions
    int nombre_transitions; // Nombre de transitions
    int etats_initiaux[MAX_TRANSITIONS]; // Tableau des états initiaux
    int nombre_etats_initiaux; // Nombre d'états initiaux
    int etats_finaux[MAX_TRANSITIONS]; // Tableau des états finaux
    int nombre_etats_finaux; // Nombre d'états finaux
    int nombre_etats; // Nombre total d'états
} Automate;

// Fonction pour lire un automate depuis un fichier
Automate lireAutomateDepuisFichier(const char *nom_fichier) {
    Automate automate;
    FILE *fichier = fopen(nom_fichier, "r");

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        exit(1);
    }

    // Initialisation des compteurs
    automate.nombre_transitions = 0;
    automate.nombre_etats_initiaux = 0;
    automate.nombre_etats_finaux = 0;

    char ligne[100];
    int etat_depart, etat_arrive;
    char etiquette_transition;
    int max_etat = 0; // Pour stocker le plus grand numéro d'état rencontré

    // Lecture du fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strstr(ligne, "#initiaux") != NULL) {
            // Traitement des états initiaux
            char *token = strtok(ligne, " ");
            while ((token = strtok(NULL, " \n")) != NULL) {
                automate.etats_initiaux[automate.nombre_etats_initiaux++] = atoi(token);
                max_etat = atoi(token) > max_etat ? atoi(token) : max_etat;
            }
        } else if (strstr(ligne, "#finaux") != NULL) {
            // Traitement des états finaux
            char *token = strtok(ligne, " ");
            while ((token = strtok(NULL, " \n")) != NULL) {
                automate.etats_finaux[automate.nombre_etats_finaux++] = atoi(token);
                max_etat = atoi(token) > max_etat ? atoi(token) : max_etat;
            }
        } else if (sscanf(ligne, " %d %d %c", &etat_depart, &etat_arrive, &etiquette_transition) == 3) {
            // Traitement des transitions
            automate.transitions[automate.nombre_transitions].etat_de_depart = etat_depart;
            automate.transitions[automate.nombre_transitions].etat_d_arrive = etat_arrive;
            automate.transitions[automate.nombre_transitions].etiquette_de_transition = etiquette_transition;
            automate.nombre_transitions++;

            // Mise à jour du plus grand numéro d'état rencontré
            max_etat = etat_depart > max_etat ? etat_depart : max_etat;
            max_etat = etat_arrive > max_etat ? etat_arrive : max_etat;
        }
    }

    // Calcul du nombre d'états en ajoutant 1 au plus grand numéro d'état rencontré
    automate.nombre_etats = max_etat + 1;

    fclose(fichier);
    return automate;
}

// Fonction pour afficher les états initiaux de l'automate
void afficherEtatsInitiaux(const Automate automate) {
    printf("Etats initiaux : ");
    for (int i = 0; i < automate.nombre_etats_initiaux; i++) {
        printf("%d ", automate.etats_initiaux[i]);
    }
    printf("\n");
}

// Fonction pour afficher les états finaux de l'automate
void afficherEtatsFinaux(const Automate automate) {
    printf("Etats finaux : ");
    for (int i = 0; i < automate.nombre_etats_finaux; i++) {
        printf("%d ", automate.etats_finaux[i]);
    }
    printf("\n");
}
// Fonction pour afficher l'alphabet de l'automate
void afficherAlphabet(const Automate automate) {
    int alphabet[MAX_TAILLE_ALPHABET] = {0};
    for (int i = 0; i < automate.nombre_transitions; i++) {
        alphabet[automate.transitions[i].etiquette_de_transition - 'a'] = 1;
    }
    printf("Alphabet de l'automate :\n");
    for (int i = 0; i < MAX_TAILLE_ALPHABET; ++i) {
        if (alphabet[i] == 1) {
            printf(" %c ", i + 'a');
        }
    }
    printf("\n");
}
// Fonction pour initialiser un tableau de booléens à false
void initialiser_tableau(bool tableau[], int taille) {
    for (int i = 0; i <= taille; i++) {
        tableau[i] = false;
    }
}

// Fonction pour marquer les états atteignables à partir d'un état initial dans un automate
void marquer_etats_atteignables(int etat_initial, const Automate automate, bool etats_atteignables[]) {
    // Marquer l'état initial comme atteignable
    etats_atteignables[etat_initial] = true;
    // Parcourir toutes les transitions de l'automate
    for (int i = 0; i < automate.nombre_transitions; i++) {
        // Si l'état de départ de la transition est atteignable, marquer l'état d'arrivée comme atteignable
        if (etats_atteignables[automate.transitions[i].etat_de_depart] == true) {
            etats_atteignables[automate.transitions[i].etat_d_arrive] = true;
        }
    }
}

// Fonction pour trouver les états atteignables dans un automate
void trouver_etats_atteignables(const Automate automate, bool etats_atteignables[]) {
    // Initialiser le tableau des états atteignables à false
    initialiser_tableau(etats_atteignables, MAX_ETAT);
    // Parcourir tous les états initiaux de l'automate
    for (int i = 0; i < automate.nombre_etats_initiaux; i++) {
        // Marquer les états atteignables à partir de chaque état initial
        marquer_etats_atteignables(automate.etats_initiaux[i], automate, etats_atteignables);
    }
}

// Fonction pour obtenir les états non atteignables dans un automate
void obtenir_etats_non_atteignables(const Automate automate, bool etats_atteignables[], bool etats_non_atteignables[]) {
    // Parcourir tous les états possibles
    for (int i = 0; i <= MAX_ETAT; i++) {
        // Les états non atteignables sont ceux qui ne sont pas marqués comme atteignables
        etats_non_atteignables[i] = !etats_atteignables[i];
    }
}

// Fonction pour générer un fichier DOT représentant l'automate et un fichier PNG correspondant
void genererFichierDotEtPNG(const Automate automate, const char *filename) {
    FILE *dotFile = fopen("automate.dot", "w");

    if (dotFile == NULL) {
        printf("Erreur lors de la creation du fichier DOT.\n");
        exit(1);
    }

    // Écriture de l'en-tête du fichier DOT
    fprintf(dotFile, "digraph Automate {\n");

    // Déclaration et initialisation des tableaux pour les états atteignables et non atteignables
    bool etats_atteignables[MAX_ETAT];
    bool etats_non_atteignables[MAX_ETAT];
    // Trouver les états atteignables
    trouver_etats_atteignables(automate, etats_atteignables);

    // Obtenir les états non atteignables
    obtenir_etats_non_atteignables(automate, etats_atteignables, etats_non_atteignables);

    // Écriture des états dans le fichier DOT avec leur coloration
    for (int i = 0; i <= MAX_ETAT; i++) {
        int estEtatInitial = 0;
        for (int j = 0; j < automate.nombre_etats_initiaux; j++) {
            if (i == automate.etats_initiaux[j]) {
                estEtatInitial = 1;
                break;
            }
        }

        int estEtatFinal = 0;
        for (int j = 0; j < automate.nombre_etats_finaux; j++) {
            if (i == automate.etats_finaux[j]) {
                estEtatFinal = 1;
                break;
            }
        }

        int estInatteignable = etats_non_atteignables[i]; // Utilisation du tableau des états non atteignables

        fprintf(dotFile, "    %d [label=\"%d\"", i, i);
        if (estInatteignable) {
            fprintf(dotFile, ", style=filled, color=\"#808080\""); // Coloration des états inatteignables en gris
        } else if (estEtatInitial) {
            fprintf(dotFile, ", style=filled, color=\"#00FF00\""); // Coloration des états initiaux en vert
        } else if (estEtatFinal) {
            fprintf(dotFile, ", style=filled, color=\"#0000FF\""); // Coloration des états finaux en bleu
        } else {
            fprintf(dotFile, ", style=filled, color=\"black\""); // Coloration des états atteignables en noir
        }

        // Couleur du texte des états en blanc
        fprintf(dotFile, ", fontcolor=\"#FFFFFF\"");

        fprintf(dotFile, "];\n");
    }

    // Écriture des transitions dans le fichier DOT
    for (int i = 0; i < automate.nombre_transitions; i++) {
        fprintf(dotFile, "    %d -> %d [label=\"%c\"];\n",
                automate.transitions[i].etat_de_depart,
                automate.transitions[i].etat_d_arrive,
                automate.transitions[i].etiquette_de_transition);
    }

    // Fermeture du fichier DOT
    fprintf(dotFile, "}\n");
    fclose(dotFile);

    // Génération du fichier PNG à partir du fichier DOT
    system("dot -Tpng automate.dot -o automate.png");

    // Affichage du fichier PNG
    system("start automate.png");
}

// Fonction récursive pour vérifier si un mot est engendré par l'automate
int estMotEngendreRecursive(const char *mot, const Automate automate, int etat_actuel, int indice_actuel) {
    // Vérifier si l'état actuel est un état final
    for (int i = 0; i < automate.nombre_etats_finaux; i++) {
        if (etat_actuel == automate.etats_finaux[i] && indice_actuel == strlen(mot)) {
            return 1; // Le mot est engendré par cet état final
        }
    }

    if (indice_actuel == strlen(mot)) {
        return 0; // Le mot est trop court pour atteindre un état final
    }

    char symbole = mot[indice_actuel];

    for (int j = 0; j < automate.nombre_transitions; j++) {
        if (automate.transitions[j].etat_de_depart == etat_actuel &&
            automate.transitions[j].etiquette_de_transition == symbole) {
            int nouvelEtat = automate.transitions[j].etat_d_arrive;
            // Appeler récursivement avec l'indice suivant et le nouvel état
            if (estMotEngendreRecursive(mot, automate, nouvelEtat, indice_actuel + 1)) {
                return 1;
            }
        }
    }

    return 0; // Aucune transition valide trouvée pour ce symbole
}

// Fonction pour vérifier si un mot est engendré par l'automate
int estMotEngendre(const char *mot, const Automate automate) {
    // Essayer de démarrer la vérification à partir de chaque état initial
    for (int i = 0; i < automate.nombre_etats_initiaux; i++) {
        if (estMotEngendreRecursive(mot, automate, automate.etats_initiaux[i], 0)) {
            return 1;
        }
    }
    return 0; // Aucun chemin valide trouvé pour le mot
}

// Fonction pour vérifier si un mot appartient à l'automate
void verifierMotAppartient(const Automate automate) {
    // Déclaration d'un tableau pour stocker le mot
    char mot[MAX_MOTS];
    // Demande à l'utilisateur d'entrer le mot à vérifier
    printf("Entrez le mot a verifier : ");
    scanf("%s", mot);
    // Vérifie si le mot est engendré par l'automate et affiche le résultat
    if (estMotEngendre(mot, automate) == 1) {
        printf("Le mot \"%s\" appartient à l'automate.\n", mot);
    } else {
        printf("Le mot \"%s\" n'appartient pas à l'automate.\n", mot);
    }
}

// Fonction pour afficher les mots engendrés par l'automate à partir d'un fichier
void afficherMotsEngendresFichier(const Automate automate, const char *nomFichier) {
    // Ouvre le fichier contenant les mots à tester
    FILE *motsFile = fopen(nomFichier, "r");
    // Vérifie si le fichier a pu être ouvert correctement
    if (motsFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s.\n", nomFichier);
        exit(1);
    }
    // Déclaration d'une variable pour stocker chaque mot lu depuis le fichier
    char mot[MAX_MOTS];
    // Lit chaque ligne du fichier et teste si les mots sont engendrés par l'automate
    while (fgets(mot, sizeof(mot), motsFile)) {
        mot[strcspn(mot, "\n")] = '\0'; // Supprime le caractère de fin de ligne s'il existe
        // Vérifie si le mot est engendré par l'automate et affiche le résultat
        if (estMotEngendre(mot, automate)) {
            printf("Le mot \"%s\" est engendre par l'automate.\n", mot);
        } else {
            printf("Le mot \"%s\" n'est pas engendre par l'automate.\n", mot);
        }
    }
    // Ferme le fichier
    fclose(motsFile);
}
// Fonction pour réaliser l'union de deux automates
Automate unionAutomates(const Automate automate1, const Automate automate2) {
    // Création d'un automate pour stocker l'union
    Automate automateUnion;
    // Initialise l'automate union avec des valeurs par défaut
    memset(&automateUnion, 0, sizeof(Automate));

    // Copie les transitions de l'automate 1 dans l'automate union
    memcpy(automateUnion.transitions, automate1.transitions, automate1.nombre_transitions * sizeof(Transition));
    automateUnion.nombre_transitions = automate1.nombre_transitions;

    // Copie les états initiaux de l'automate 1 dans l'automate union
    memcpy(automateUnion.etats_initiaux, automate1.etats_initiaux, sizeof(automate1.etats_initiaux));
    automateUnion.nombre_etats_initiaux = automate1.nombre_etats_initiaux;

    // Copie les états finaux de l'automate 1 dans l'automate union
    memcpy(automateUnion.etats_finaux, automate1.etats_finaux, automate1.nombre_etats_finaux * sizeof(int));
    automateUnion.nombre_etats_finaux = automate1.nombre_etats_finaux;

    // Ajoute les transitions de l'automate 2 à l'automate union
    automateUnion.transitions[automateUnion.nombre_transitions].etat_de_depart = automate2.transitions[0].etat_de_depart;
    automateUnion.transitions[automateUnion.nombre_transitions].etat_d_arrive = automate2.transitions[0].etat_d_arrive + automate1.nombre_transitions;
    automateUnion.transitions[automateUnion.nombre_transitions].etiquette_de_transition = automate2.transitions[0].etiquette_de_transition;
    automateUnion.nombre_transitions++;

    // Ajoute les autres transitions de l'automate 2 à l'automate union
    int j = 1;
    while (j < automate2.nombre_transitions) {
        automateUnion.transitions[automateUnion.nombre_transitions].etat_de_depart = automate2.transitions[j].etat_de_depart + automate1.nombre_transitions;
        automateUnion.transitions[automateUnion.nombre_transitions].etat_d_arrive = automate2.transitions[j].etat_d_arrive + automate1.nombre_transitions;
        automateUnion.transitions[automateUnion.nombre_transitions].etiquette_de_transition = automate2.transitions[j].etiquette_de_transition;
        automateUnion.nombre_transitions++;
        j++;
    }

    // Ajoute les états finaux de l'automate 2 à l'automate union
    for (int i = 0; i < automate2.nombre_etats_finaux; i++) {
        automateUnion.etats_finaux[automateUnion.nombre_etats_finaux++] = automate2.etats_finaux[i] + automate1.nombre_transitions;
    }

    automateUnion.nombre_etats_finaux += automate2.nombre_etats_finaux;

    return automateUnion;
}

// Fonction pour réaliser l'étoile d'un automate
Automate etoileAutomate(const Automate automate) {
    // Copie de l'automate initial
    Automate automateEtoile = automate;

    // Ajoute les transitions pour la boucle d'étoile
    int j = 0;
    while (j < automateEtoile.nombre_etats_initiaux) {
        for (int i = 0; i < automateEtoile.nombre_etats_finaux; i++) {
            automateEtoile.transitions[automateEtoile.nombre_transitions].etat_de_depart = automate.etats_initiaux[j];
            automateEtoile.transitions[automateEtoile.nombre_transitions].etat_d_arrive = automateEtoile.etats_finaux[i];
            automateEtoile.transitions[automateEtoile.nombre_transitions].etiquette_de_transition = '$'; // Transition spéciale
            automateEtoile.nombre_transitions++;
        }
        j++;
    }

    // Ajoute les transitions pour l'étoile inversée
    j = 0;
    while (j < automateEtoile.nombre_etats_initiaux) {
        for (int i = 0; i < automateEtoile.nombre_etats_finaux; i++) {
            automateEtoile.transitions[automateEtoile.nombre_transitions].etat_de_depart = automateEtoile.etats_finaux[i];
            automateEtoile.transitions[automateEtoile.nombre_transitions].etat_d_arrive = automate.etats_initiaux[j];
            automateEtoile.transitions[automateEtoile.nombre_transitions].etiquette_de_transition = '$'; // Transition spéciale
            automateEtoile.nombre_transitions++;
        }
        j++;
    }

    return automateEtoile;
}
// Fonction pour calculer l'intersection de deux automates
Automate intersectionAutomates(const Automate automate1, const Automate automate2) {
    // Création d'un automate pour stocker l'intersection
    Automate automateIntersection;
    // Initialise l'automate intersection avec des valeurs par défaut
    memset(&automateIntersection, 0, sizeof(Automate));

    // Ajoute les états initiaux de l'intersection
    for (int i = 0; i < automate1.nombre_etats_initiaux; i++) {
        for (int j = 0; j < automate2.nombre_etats_initiaux; j++) {
            automateIntersection.etats_initiaux[automateIntersection.nombre_etats_initiaux++] = automate1.etats_initiaux[i] + automate2.etats_initiaux[j];
        }
    }

    // Ajoute les états finaux de l'intersection
    for (int i = 0; i < automate1.nombre_etats_finaux; i++) {
        for (int j = 0; j < automate2.nombre_etats_finaux; j++) {
            // Vérifie si les états finaux sont égaux
            if (automate1.etats_finaux[i] != automate2.etats_finaux[j]) {
                // Si non égaux, ajoute les deux états finaux dans l'intersection
                automateIntersection.etats_finaux[automateIntersection.nombre_etats_finaux++] = automate1.etats_finaux[i];
                automateIntersection.etats_finaux[automateIntersection.nombre_etats_finaux++] = automate2.etats_finaux[i];
            } else {
                // Si égaux, ajoute juste un des états finaux
                automateIntersection.etats_finaux[automateIntersection.nombre_etats_finaux++] = automate1.etats_finaux[i];
            }
        }
    }

    // Ajoute les transitions de l'intersection
    for (int i = 0; i < automate1.nombre_transitions; i++) {
        for (int j = 0; j < automate2.nombre_transitions; j++) {
            char etiq1 = automate1.transitions[i].etiquette_de_transition;
            char etiq2 = automate2.transitions[j].etiquette_de_transition;
            // Vérifie si les transitions des deux automates ont la même étiquette et arrivent à des états différents
            if (automate1.transitions[i].etat_d_arrive != automate1.transitions[i].etat_de_depart &&
                automate2.transitions[j].etat_d_arrive != automate2.transitions[j].etat_de_depart &&
                etiq1 == etiq2) {
                // Si oui, ajoute la transition dans l'intersection
                int etiq = etiq1;
                automateIntersection.transitions[automateIntersection.nombre_transitions].etat_de_depart = automate1.transitions[i].etat_de_depart;
                automateIntersection.transitions[automateIntersection.nombre_transitions].etat_d_arrive = automate1.transitions[i].etat_d_arrive;
                automateIntersection.transitions[automateIntersection.nombre_transitions].etiquette_de_transition = etiq;
                automateIntersection.nombre_transitions++;
            }
        }
    }

    return automateIntersection;
}
// Fonction pour supprimer les transitions epsilon d'un automate
Automate suppressionEpsilon(Automate a) {
    Automate result; // Automate résultant après la suppression des transitions epsilon
    result.nombre_transitions = 0;
    result.nombre_etats_initiaux = 0;
    result.nombre_etats_finaux = 0;
    result.nombre_etats = 0;

    // Allocation de mémoire pour les transitions
    result.nombre_transitions = a.nombre_transitions;
    memcpy(result.transitions, a.transitions, a.nombre_transitions * sizeof(Transition));

    // Allocation de mémoire pour les états initiaux et finaux
    for (int i = 0; i < a.nombre_etats_initiaux; i++) {
        result.etats_initiaux[i] = a.etats_initiaux[i];
        result.nombre_etats_initiaux++;
    }
    for (int i = 0; i < a.nombre_etats_finaux; i++) {
        result.etats_finaux[i] = a.etats_finaux[i];
        result.nombre_etats_finaux++;
    }

    int tr = 0; // Compteur pour les transitions dans le résultat
    // Parcourir toutes les transitions de l'automate d'entrée
    for (int k = 0; k < a.nombre_transitions; k++) {
        // Ajouter les transitions non-epsilon à la structure résultante
        if (a.transitions[k].etiquette_de_transition != '$') {
            result.transitions[tr] = a.transitions[k];
            tr++;
        }
    }
    result.nombre_transitions = tr; // Mettre à jour le nombre de transitions dans le résultat

    // Suppression des états finaux redondants générés par les transitions epsilon
    for (int k = 0; k < a.nombre_transitions; k++) {
        for (int i = 0; i < a.nombre_etats_finaux; i++) {
            if (a.transitions[k].etat_de_depart == a.etats_finaux[i] && a.transitions[k].etiquette_de_transition == '$') {
                // Vérifier si l'état final est déjà dans la liste des états finaux du résultat
                bool found = false;
                for (int j = 0; j < result.nombre_etats_finaux; j++) {
                    if (result.etats_finaux[j] == a.transitions[k].etat_d_arrive) {
                        found = true;
                        break;
                    }
                }
                // Ajouter l'état final généré par une transition epsilon s'il n'est pas déjà présent
                if (!found) {
                    result.etats_finaux[result.nombre_etats_finaux] = a.transitions[k].etat_d_arrive;
                    result.nombre_etats_finaux++;
                }
            }
        }
    }

    return result; // Retourner l'automate résultant après la suppression des transitions epsilon
}

// Fonction pour déterminiser un automate
Automate determinisation(Automate a) {
    Automate resultat; // Automate résultant après la déterminisation
    resultat.nombre_transitions = 0;
    resultat.nombre_etats_initiaux = 0;
    resultat.nombre_etats_finaux = 0;
    resultat.nombre_etats = 0;

    int i, j, k, found;
    char etiquete[MAX_TRANSITIONS][MAX_TAILLE_ALPHABET]; // Tableau pour stocker les étiquettes des transitions
    char nb_etiquete = 0;

    // Recherche des étiquettes uniques dans les transitions de l'automate d'entrée
    for (i = 0; i < a.nombre_transitions; i++) {
        found = 0;
        for (k = 0; k < nb_etiquete; k++) {
            if (etiquete[k][0] == a.transitions[i].etiquette_de_transition) {
                found = 1;
                break;
            }
        }
        if (!found) {
            etiquete[nb_etiquete][0] = a.transitions[i].etiquette_de_transition;
            nb_etiquete++;
        }
    }

    char etats[MAX_TRANSITIONS][MAX_ETAT][2]; // Tableau pour stocker les états de l'automate résultant
    int nb_etat = 0, nb_current_etat = 0; // Compteurs pour les états dans l'automate résultant
    char tmp[MAX_TRANSITIONS][2]; // Tableau temporaire pour stocker les états suivants
    int nbr_etat[MAX_TRANSITIONS], tmp_var = 0; // Tableaux pour le nombre d'états et les états temporaires
    char etat_courant[MAX_TRANSITIONS * MAX_ETAT * 2] = ""; // Chaîne de caractères pour l'état courant
    int mapping[MAX_ETAT]; // Tableau pour mapper les anciens numéros d'états aux nouveaux numéros d'états

    // Initialiser le tableau de mappage à -1
    for (i = 0; i < MAX_ETAT; i++) {
        mapping[i] = -1;
    }

    // Ajouter les états initiaux de l'automate d'entrée à l'état initial de l'automate résultant
    for (i = 0; i < a.nombre_etats_initiaux; i++) {
        etats[nb_etat][i][0] = a.etats_initiaux[i] + '0';
        strcat(etat_courant, etats[nb_etat][i]);
        strcat(etat_courant, ",");
    }

    etat_courant[strlen(etat_courant) - 1] = '\0';
    resultat.etats_initiaux[resultat.nombre_etats_initiaux] = atoi(etat_courant); // Enregistrer l'état initial
    mapping[atoi(etat_courant)] = resultat.nombre_etats; // Mapper l'ancien numéro d'état au nouveau numéro d'état
    nbr_etat[nb_etat] = a.nombre_etats_initiaux;
    nb_etat++;
    resultat.nombre_etats_initiaux++;
    resultat.nombre_etats++;

    // Tant qu'il y a des états à traiter
    while (nb_current_etat < nb_etat) {
        int final = 0; // Variable pour indiquer si l'état courant est final
        // Copier les états de l'état courant
        for (j = 0; j < nbr_etat[nb_current_etat]; j++) {
            strcpy(etats[nb_etat][j], etats[nb_current_etat][j]);
            // Vérifier si l'un des états de l'état courant est final
            for (k = 0; k < a.nombre_etats_finaux; k++) {
                if (etats[nb_current_etat][j][0] == (a.etats_finaux[k] + '0')) {
                    final = 1;
                    break;
                }
            }
        }

        // Si l'état courant est final, l'ajouter aux états finaux de l'automate résultant
        if (final) {
            char etat_courant[MAX_TRANSITIONS * MAX_ETAT * 2] = "";
            for (j = 0; j < nbr_etat[nb_current_etat]; j++) {
                strcat(etat_courant, etats[nb_current_etat][j]);
                strcat(etat_courant, ",");
            }
            etat_courant[strlen(etat_courant) - 1] = '\0';
            resultat.etats_finaux[resultat.nombre_etats_finaux++] = mapping[atoi(etat_courant)]; // Enregistrer l'état final
        }

        // Pour chaque étiquette d'une transition dans l'automate d'entrée
        for (i = 0; i < nb_etiquete; i++) {
            tmp_var = 0;
            // Pour chaque état de l'état courant
            for (j = 0; j < nbr_etat[nb_current_etat]; j++) {
                // Pour chaque transition dans l'automate d'entrée
                for (k = 0; k < a.nombre_transitions; k++) {
                    // Si la transition correspond à l'état courant et à l'étiquette actuelle
                    if ((etats[nb_current_etat][j][0] == a.transitions[k].etat_de_depart + '0') && (etiquete[i][0] == a.transitions[k].etiquette_de_transition)) {
                        tmp[tmp_var][0] = a.transitions[k].etat_d_arrive + '0';
                        tmp_var++;
                    }
                }
            }

            // Supprimer les doublons des états suivants
            int size = tmp_var;
            char uniqueArr[size][2];
            int uniqueSize = 0;
            int isDuplicate = 0;
            for (k = 0; k < size; k++) {
                isDuplicate = 0;
                for (j = 0; j < uniqueSize; j++) {
                    if (strcmp(tmp[k], uniqueArr[j]) == 0) {
                        isDuplicate = 1;
                        break;
                    }
                }
                if (!isDuplicate) {
                    strcpy(uniqueArr[uniqueSize], tmp[k]);
                    uniqueSize++;
                }
            }

            // Trier les états suivants
            for (k = 0; k < uniqueSize - 1; k++) {
                for (j = 0; j < uniqueSize - k - 1; j++) {
                    if (strcmp(uniqueArr[j], uniqueArr[j + 1]) > 0) {
                        char temp[2];
                        strcpy(temp, uniqueArr[j]);
                        strcpy(uniqueArr[j], uniqueArr[j + 1]);

                    }
                }
            }

            // Vérification de la présence de l'état courant dans les états déjà explorés
            int found = 0;
            for (j = 0; j < nb_etat; j++) {
                for (k = 0; k < uniqueSize; k++) {
                    if ((uniqueArr[k][0] != etats[j][k][0]) || nbr_etat[j] != uniqueSize) {
                        break;
                    }
                    else if (k == (uniqueSize - 1)) {
                        found = 1;
                    }
                }
            }

            // Si l'état courant n'a pas été rencontré auparavant, le traiter
            if (uniqueSize != 0) {
                char etat_courant[MAX_TRANSITIONS * MAX_ETAT * 2] = "", etat_suivant[MAX_TRANSITIONS * MAX_ETAT * 2] = "";

                // Construction de la chaîne représentant l'état suivant
                for (j = 0; j < uniqueSize; j++) {
                    strcat(etat_suivant, uniqueArr[j]);
                    strcat(etat_suivant, ",");
                }
                etat_suivant[strlen(etat_suivant) - 1] = '\0';

                // Construction de la chaîne représentant l'état courant
                for (j = 0; j < nbr_etat[nb_current_etat]; j++) {
                    strcat(etat_courant, etats[nb_current_etat][j]);
                    strcat(etat_courant, ",");
                }
                etat_courant[strlen(etat_courant) - 1] = '\0';

                // Vérification si l'état courant est déjà mappé
                int etat_courant_mapped = mapping[atoi(etat_courant)];

                // Si l'état courant n'est pas mappé, mapper et incrémenter le nombre d'états
                if (etat_courant_mapped == -1) {
                    mapping[atoi(etat_courant)] = resultat.nombre_etats;
                    etat_courant_mapped = resultat.nombre_etats;
                    resultat.nombre_etats++;
                }

                // Vérification si l'état suivant est déjà mappé
                int etat_suivant_mapped = mapping[atoi(etat_suivant)];
                if (etat_suivant_mapped == -1) {
                    mapping[atoi(etat_suivant)] = resultat.nombre_etats;
                    etat_suivant_mapped = resultat.nombre_etats;
                    resultat.nombre_etats++;
                }

                // Avant d'ajouter une transition, vérification si elle pointe vers le même état
                if (strcmp(etat_courant, etat_suivant) != 0) {
                    // Ajout de la transition à l'automate résultant
                    resultat.transitions[resultat.nombre_transitions].etat_de_depart = etat_courant_mapped;
                    resultat.transitions[resultat.nombre_transitions].etat_d_arrive = etat_suivant_mapped;
                    resultat.transitions[resultat.nombre_transitions].etiquette_de_transition = etiquete[i][0];
                    resultat.nombre_transitions++;
                }

                // Si l'état suivant est final, l'ajouter aux états finaux de l'automate résultant
                for (j = 0; j < a.nombre_etats_finaux; j++) {
                    if (atoi(etat_suivant) == a.etats_finaux[j]) {
                        resultat.etats_finaux[resultat.nombre_etats_finaux++] = etat_suivant_mapped;
                        break;
                    }
                }

                // Si l'état courant est final, l'ajouter aux états finaux de l'automate résultant
                for (j = 0; j < a.nombre_etats_finaux; j++) {
                    if (atoi(etat_courant) == a.etats_finaux[j]) {
                        resultat.etats_finaux[resultat.nombre_etats_finaux++] = etat_courant_mapped;
                        break;
                    }
                }

                // Si l'état n'a pas été rencontré précédemment, l'ajouter pour traitement ultérieur
                if (!found) {
                    for (j = 0; j < uniqueSize; j++) {
                        strcpy(etats[nb_etat][j], uniqueArr[j]);
                    }
                    nbr_etat[nb_etat] = uniqueSize;
                    nb_etat++;
                }
            }
        }
        nb_current_etat++;
    }

    return resultat;
}
Automate renversement(Automate a) {
    // Inverser les transitions
    for (int i = 0; i < a.nombre_transitions; i++) {
        // Échanger les états de départ et d'arrivée
        int etat_depart = a.transitions[i].etat_de_depart;
        a.transitions[i].etat_de_depart = a.transitions[i].etat_d_arrive;
        a.transitions[i].etat_d_arrive = etat_depart;
    }

    // Inverser les états initiaux et finaux
    for (int i = 0; i < a.nombre_etats_initiaux; i++) {
        // Échanger les états initiaux et finaux
        int temp = a.etats_initiaux[i];
        a.etats_initiaux[i] = a.etats_finaux[i];
        a.etats_finaux[i] = temp;
    }

    // Mettre à jour le nombre d'états initiaux et finaux
    int temp_nombre_etats_initiaux = a.nombre_etats_initiaux;
    a.nombre_etats_initiaux = a.nombre_etats_finaux;
    a.nombre_etats_finaux = temp_nombre_etats_initiaux;

    return a;
}

// Implémentation de la fonction pour afficher l'automate
void afficherAutomate(const Automate automate) {
    // Afficher le nombre de transitions
    printf("Nombre de transitions : %d\n", automate.nombre_transitions);
    printf("Transitions :\n");
    // Parcourir les transitions et les afficher
    for (int i = 0; i < automate.nombre_transitions; i++) {
        printf("De l'etat %d a l'etat %d avec l'etiquette '%c'\n",
               automate.transitions[i].etat_de_depart,
               automate.transitions[i].etat_d_arrive,
               automate.transitions[i].etiquette_de_transition);
    }
    // Afficher le nombre d'états initiaux
    printf("Nombre d'etats initiaux : %d\n", automate.nombre_etats_initiaux);
    printf("Etats initiaux : ");
    // Afficher les états initiaux
    for (int i = 0; i < automate.nombre_etats_initiaux; i++) {
        printf("{%d }", automate.etats_initiaux[i]);
    }
    printf("\n");
    // Afficher le nombre d'états finaux
    printf("Nombre d'etats finaux : %d\n", automate.nombre_etats_finaux);
    printf("Etats finaux : ");
    // Afficher les états finaux
    for (int i = 0; i < automate.nombre_etats_finaux; i++) {
        printf("{%d} ", automate.etats_finaux[i]);
    }
    printf("\n");
}
// Fonction pour afficher le menu des opérations sur les automates
void afficherMenu(Automate automate1, Automate automate2) {
        int choix;
        Automate result;
        // Affichage du menu tant que l'utilisateur ne choisit pas de quitter
        do {
            printf("\n");
            printf("Bienvenue dans le projet de manipulation d'automates realise par :\n\n");
            printf("                      ----> SAAD BARHROUJ                          \n\n");
            printf("***************************************************************\n");
            printf("*                                                            *\n");
            printf("*                  Menu Principal                            *\n");
            printf("*                                                            *\n");
            printf("**************************************************************\n\n");
            printf("**************     Operation sur l automate 1   **************\n\n");
            printf("*    1. Afficher l'alphabet de l'automate 1                  *\n");
            printf("*    2. Afficher les etats initiaux de l'automate 1          *\n");
            printf("*    3. Afficher les etats finaux de l'automate 1            *\n");
            printf("*    4. Generer fichier DOT et afficher PNG de l'automate 1  *\n");
            printf("*    5. Verifier si un mot appartient a l'automate 1         *\n");
            printf("*    6. Verifier les mots d'un fichier avec l'automate 1     *\n\n");
            printf("**************     Operation sur l automatec 2  **************\n\n");
            printf("*    7. Afficher l'alphabet de l'automate 2                  *\n");
            printf("*    8. Afficher les etats initiaux de l'automate 2          *\n");
            printf("*    9. Afficher les etats finaux de l'automate 2            *\n");
            printf("*    10. Generer fichier DOT et afficher PNG de l'automate 2 *\n");
            printf("*    11. Verifier si un mot appartient a l'automate 2        *\n");
            printf("*    12. Verifier les mots d'un fichier avec l'automate 2    *\n\n");
            printf("*******     Operation sur les deux automates 1 et 2    *******\n\n");
            printf("*    13. Union des automates                                 *\n");
            printf("*    14. Intersection des automates                          *\n");
            printf("*    15. Etoile de l'automate                                *\n");
            printf("*    16. Suppression de l'epsilon de l'automate              *\n");
            printf("*    17. Determinisation de l'automate                       *\n");
            printf("*    18. Minimisation de l'automate                          *\n");
            printf("*******************      Au revoi    *************************\n");
            printf("*    0. Quitter                                              *\n");
            printf("*                                                            *\n");
            printf("**************************************************************\n");
            printf("                   Entrez votre choix :    ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                afficherAlphabet(automate1);
                break;
            case 2:
                afficherEtatsInitiaux(automate1);
                break;
            case 3:
                afficherEtatsFinaux(automate1);
                break;
            case 4:
                genererFichierDotEtPNG(automate1, "automate1");
                break;
            case 5:
                verifierMotAppartient(automate1);
                break;
            case 6: {
                char nomFichier[MAX_MOTS];
                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                afficherMotsEngendresFichier(automate1, nomFichier);
                break;
            }
            case 7:
                afficherAlphabet(automate2);
                break;
            case 8:
                afficherEtatsInitiaux(automate2);
                break;
            case 9:
                afficherEtatsFinaux(automate2);
                break;
            case 10:
                genererFichierDotEtPNG(automate2, "automate2");
                break;
            case 11:
                verifierMotAppartient(automate2);
                break;
            case 12: {
                char nomFichier[MAX_MOTS];
                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                afficherMotsEngendresFichier(automate2, nomFichier);
                break;
            }
            case 13:
                afficherAutomate(unionAutomates(automate1, automate2));
                genererFichierDotEtPNG(unionAutomates(automate1, automate2), "union_automates");
                break;
            case 14:
                afficherAutomate(intersectionAutomates(automate1, automate2));
                genererFichierDotEtPNG(intersectionAutomates(automate1, automate2), "concatenation_automates");
                break;
            case 15:
                automate1 = etoileAutomate(automate1);
                afficherAutomate(automate1);
                genererFichierDotEtPNG(automate1, "etoile_automate");
                break;
            case 16:
                automate1 = suppressionEpsilon(automate1);
                afficherAutomate(automate1);
                genererFichierDotEtPNG(automate1, "sans_epsilon_automate");
                break;
            case 17:
                automate1 = determinisation(automate1);
                afficherAutomate(automate1);
                genererFichierDotEtPNG(automate1, "determinise_automate");
                break;
            case 18:
                result = renversement(automate1);
                result = determinisation(result);
                result = renversement(result);
                result = determinisation(result);
                result.etats_initiaux[0]=result.transitions[0].etat_de_depart;

                genererFichierDotEtPNG(result, "minimiser_automate");
                afficherAutomate(result);
                break;
            case 0:
                printf("Au revoir!\n");
                break;
            default:
                printf("Choix invalide!\n");
                break;
        }
    } while (choix != 0);
}

// Fonction principale du programme
int main() {
    Automate automate1 = lireAutomateDepuisFichier("automate1.txt");
    Automate automate2 = lireAutomateDepuisFichier("automate2.txt");
    afficherMenu(automate1, automate2);
    return 0;
}

