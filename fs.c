/**
 * UQAM - Hiver 2017 - INF3172 - Groupe 10 - TP1
 *
 * Fichier fs.c - Fichier source  C  d'un programme de systeme de fichiers base
 *               sur les i-nodes. Les i-nodes du systeme de fichiers supportent
 *               seulement  l'indirection  simple.  Seul l'i-node associe  a un  
 *               fichier n'est charge en memoire lors de son ouverture.
 *
 *               Le systeme de fichiers est compose des fichiers suivants :
 *               1 - un fichier binaire de disque de 512 Ko (32 000 blocs de 
 *                   16 octets).
 *               2 - un fichier binaire de blocs libres de 32 Ko
 *               3 - un fichier binaire enregistrant les donnees des i-nodes.
 *               4 - un fichier binaire enregistrant les noms des repertoires. 
 *
 *              Entrez "./fs NOM_FICHIER_COMMANDES" dans le repertoire contenant
 *              l'executable   "fs"   et   le   fichier   de   commandes   texte 
 *              "NOM_FICHIER_COMMANDES"   pour    executer     le     programme. 
 *              NOM_FICHIER_COMMANDES  doit exister.  Si Le  fichier  de  disque 
 *              n'est pas  present dans le repertoire d'execution  du programme, 
 *              le programme genere une fichier de  disque vide  et remplace les
 *              fichiers de  blocs libres,  d'i-nodes  et de repertoires par des 
 *              fichiers vides. Si le fichier  de  disque  existe, le  programme
 *              assume que  les  fichiers  de  blocs  libres,  d'i-nodes  et  de 
 *              repertoires sont tous coherents les uns avec les autres. 
 *
 *              Les commandes valides d'un fichier de commandes sont :
 *
 *              1 - creation_repertoire NOM_REPERTOIRE
 *              2 - suppression_repertoire NOM_REPERTOIRE
 *              3 - creation_fichier NOM_FICHIER DONNEES
 *              4 - suppression_fichier NOM_FICHIER
 *              5 - lire_fichier NOM_FICHIER
 *
 * @Auteur      Chretien Alexis - CHRA25049209
 * @Version     12 mars 2017
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Constantes
 */
#define   WINDOWS_FIN_LIGNE         0xD
#define   TAILLE_MAX_CHAINE         1024
#define   NB_MAX_BLOCS              32000
#define   TAILLE_BLOC               16
#define   NB_BLOCS_DIRECTS          8
#define   TAILLE_MAX_NOM_FICHIER    40
#define   TAILLE_MAX_FICHIER        256   
#define   ERREUR_NB_ARGUMENTS       1
#define   ERREUR_NOM_FICHIER        2
#define   ERREUR_OUVERTURE_FICHIER  3
#define   ERREUR_REP_EXISTE_DEJA    4
#define   ERREUR_REP_PARENT         5 
#define   ERREUR_TAILLE_NOM_REP     6
#define   ERREUR_REP_EXISTE_PAS     7
#define   ERREUR_TAILLE_FICHIER     8
#define   ERREUR_DISQUE_PLEIN       9
#define   ERREUR_FICHIER_VIDE       10
#define   ERREUR_FICHIER_EXISTE     11
#define   ERREUR_TAILLE_NOM_FICHIER 12
#define   ERREUR_FICHIER_PARENT     13
#define   ERREUR_FICHIER_EXISTE_PAS 14
#define   ERREUR_LECTURE            15

const char *NOM_FICHIER_VIDE         = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
const char *BLOC_VIDE                = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
const char *NOM_FICHIER_DISQUE       = "disque.dat";
const char *NOM_FICHIER_REP          = "repertoires.dat";
const char *NOM_FICHIER_INODES       = "inodes.dat";
const char *NOM_FICHIER_BLOCS_LIBRES = "blocs_libres.dat";

/**
 * Structures
 */
/**
 * Inode - structure enregistrant les informations d'un fichier. 
 *
 * @membre  nomFichier         Le nom du fichier associe a l'i-node (son chemin 
 *                             absolue).
 * @membre  noBloc             Un tableau des numeros de blocs du disque contenant
 *                             les donnees du fichier associe a l'i-node.
 * @membre  noBlocIndirection  Le numero d'un bloc d'indirection. Ce bloc (16 octets)
 *                             contient huit nombres de deux octets. Ces huit
 *                             nombres sont les numero de blocs supplementaires 
 *                             enregistrant des donnees du fichier associe a 
 *                             l'i-node. Un fichier peut donc contenir 
 *                             8 * 16 octets + 8 * 16 octets = 256 octets. 
 */ 
typedef struct {
    char nomFichier[TAILLE_MAX_NOM_FICHIER];
    int  noBloc[NB_BLOCS_DIRECTS];
    int  noBlocIndirection;
} Inode;

/**
 * Fonction qui renvoie un pointeur sur un nouvel i-node. 
 *
 * Les valeurs des numeros de bloc et de numero de bloc d'indirections sont
 * initialisees a -1.
 *
 * @param  nomFichier       Le nom du fichier associe a l'i-node.
 * @return unInode          Pointeur pointant sur le nouvel i-node.
 */
Inode *nouveauInode(char nomFichier[TAILLE_MAX_NOM_FICHIER]) {
    
    int i;

    Inode *unInode;
    unInode = malloc(sizeof(Inode));

    strcpy(unInode->nomFichier, nomFichier);

    for (i = 0 ; i < 8 ; ++i) {
        unInode->noBloc[i]  = -1;
    }
    unInode->noBlocIndirection = -1;

    return unInode;
} 

/**
 * Fonction d'affichage de message d'erreur sur le canal stderr.
 *
 * Les erreurs liees a de mauvais arguments de lancement du programme
 * causent l'arret de l'execution. Les erreurs liees a des commandes
 * non-valides affiche simplement l'erreur sur le canal d'erreur, sans
 * causer l'arret du programme. 
 *
 * @param  noErreur         Le numero de l'erreur.
 * @param  chaine           Chaine de caracteres qui precise la
 *                          nature de l'erreur (un nom de fichier
 *                          ou de repertoire).
 * @return
 */
void afficherMessageErreur(int noErreur, char *chaine) {

    switch (noErreur) {

        case 1 :  fprintf(stderr,"Erreur, nombre d'arguments invalide.\n"
                                 "Utilisation : ./fs nom_fichier\n");
                  exit(1);
        case 2 :  fprintf(stderr, "Erreur, fichier en argument \"%s\" non-existant.\n", chaine);
                  exit(1);
        case 3 :  fprintf(stderr, "Erreur lors de l'ouverture d'un fichier : %s\n", strerror(errno));
                  exit(1);
        case 4 :  fprintf(stderr, "Erreur, creation du repertoire \"%s\" impossible : le repertoire existe deja.\n", chaine);
                  break;
        case 5 :  fprintf(stderr, "Erreur, creation du repertoire \"%s\" impossible : le repertoire parent n'existe pas.\n", chaine);
                  break;
        case 6 :  fprintf(stderr, "Erreur, creation du repertoire \"%s\" impossible : le chemin absolue depasse 38 caracteres.\n", chaine);
                  break;
        case 7 :  fprintf(stderr, "Erreur, suppression du repertoire \"%s\" impossible : le repertoire n'existe pas.\n", chaine);
                  break;
        case 8 :  fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : la taille du fichier depasse 256 octets.\n", chaine);
                  break;
        case 9 :  fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : la taille du fichier est plus grande que l'espace "
                                  "disque restante.\n", chaine);
                  break;
        case 10 : fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : le fichier est vide.\n", chaine);
                  break;
        case 11 : fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : le fichier existe deja.\n", chaine);
                  break;
        case 12 : fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : le chemin absolue depasse 40 caracteres.\n", chaine);
                  break;
        case 13 : fprintf(stderr, "Erreur, creation du fichier \"%s\" impossible : le repertoire parent n'existe pas.\n", chaine);
                  break;
        case 14 : fprintf(stderr, "Erreur, suppression du fichier \"%s\" impossible : le fichier n'existe pas.\n", chaine);
                  break;
        case 15 : fprintf(stderr, "Erreur, lecture du fichier \"%s\" impossible : le fichier n'existe pas.\n", chaine);
        default : break;
    }
}

/**
 * Fonction qui verifie si les arguments entrees lors du lancement du programme
 * sont valides.
 *
 * Pour etre valides, les arguments doivent etre deux. Le second argument doit
 * etre le nom d'un fichier existant dans le repertoire local de l'execution.
 * La fonction appelle "afficherMessageErreur" si les arguments sont invalides.
 *
 * @param  nbArguments      Le nombre d'arguments entres, incluant le nom de
 *                          l'executable.
 * @param  arguments        Les arguments entres.
 * @return
 */
void validerArguments(int nbArguments, char *arguments[]) {

    FILE *fichierCommandes;

    if (nbArguments != 2) {
        afficherMessageErreur(ERREUR_NB_ARGUMENTS, NULL);
    }
    else {
        fichierCommandes = fopen(arguments[1], "r");
        if (fichierCommandes == NULL) {
            afficherMessageErreur(ERREUR_NOM_FICHIER, arguments[1]);
        }
        else {
            fclose(fichierCommandes);
        }
    }
}

/**
 * Fonction qui genere un fichier binaire de 512 ko vide (32 000 blocs de
 * 16 octets). 
 *
 * Le caractere "null" ('\0') est utilise pour simuler un octet vide.   
 *
 * @return
 */
void genererFichierDisqueVide() {
    
    int i;
    FILE *fichierDisque;
    
    fichierDisque = fopen(NOM_FICHIER_DISQUE, "wb");

    for (i = 0 ; i < NB_MAX_BLOCS ; ++i) {
        fwrite((char *) BLOC_VIDE, TAILLE_BLOC, 1, fichierDisque);
    }
    fclose(fichierDisque);
}

/**
 * Fonction qui genere un fichier binaire historisant les blocs libres
 * du fichier disque. 
 *
 * Chaque entree du fichier est composee d'un booleen (un octet). Tous 
 * ces octets sont initialises a "true" - le disque est vide. 
 *
 * @return
 */
void genererFichierBlocsLibres() {
    
    bool vrai = true;
    int i;
    FILE *fichierBlocsLibres;

    fichierBlocsLibres = fopen(NOM_FICHIER_BLOCS_LIBRES, "wb");

    for (i = 0 ; i < NB_MAX_BLOCS ; ++i) {
        fwrite(&vrai, sizeof(bool), 1, fichierBlocsLibres);
    }
    fclose(fichierBlocsLibres);
}

/**
 * Fonction qui verifie si le fichier disque existe. 
 *
 * S'il n'existe pas, la fonction fait appelle a "genererFichierDisqueVide"
 * et "genererFichierBlocsLibres" pour generer de nouveau fichiers, supprime
 * les fichiers de repertoires et d'i-nodes s'ils existent et en cree de nouveaux
 * en les laissant vides. 
 *
 * @return
 */
void verifierFichierDisque() {
    
    FILE *fichierDisque;

    fichierDisque = fopen(NOM_FICHIER_DISQUE, "r");
    
    if (fichierDisque == NULL) {

        genererFichierDisqueVide();
        genererFichierBlocsLibres();
        remove(NOM_FICHIER_REP);
        remove(NOM_FICHIER_INODES);
        system("touch repertoires.dat");
        system("touch inodes.dat");
    }
    else {
        fclose(fichierDisque);
    }
}

/**
 * Fonction qui verifie si une chaine de caractere est le prefixe d'un autre.
 *
 * @param  chaine1     La chaine potentiellement prefixe.
 * @param  chaine2     La chaine qui contient potentiellement chaine1 en debut.
 * @return             "true" si chaine1 est prefixe de chaine2, "false" sinon.
 */
bool estPrefixe (char *chaine1, char *chaine2) {

    int i;

    for (i = 0 ; i < strlen(chaine1) ; ++i) {
        if (chaine1[i] != chaine2[i]) {
            return false;
        }
     }
    return true;
}

/**
 * Fonction qui verifie si un repertoire est le repertoire parent (et non le
 * repertoire grand-parent, grand-grand-parent, etc) d'un fichier ou d'un 
 * repertoire. 
 *
 * repertoireParent est le repertoire parent de nomFichier si :
 *
 * 1 - repertoireParent est le prefixe de nomFichier ET
 * 2 - le premier caractere d'index plus grand que l'index maximal
 *     de repertoireParent est le caractere '/' ET
 * 3 - de ce caractere a la fin de nomFichier, il n'y a aucun autre
 *     caractere '/'.
 *
 * @param  repertoireParent    Le nom du repertoire potentiellement parent a
 *                             nomFichier.
 * @param  nomFichier          Le nom du fichier ou repertoire potentiellement
 *                             enfant a repertoireParent. 
 * @return                     "true" si repertoireParent est le repertoire parent
 *                             direct de nomFichier, "false" sinon.
 */
bool estRepertoireParent(char *repertoireParent, char *nomFichier) {

    char *nomRepertoireLocal;
    bool estRepertoireParent = true;
    
    nomRepertoireLocal = nomFichier + strlen(repertoireParent);

    if (nomRepertoireLocal[0] != '/' 
            || strpbrk(++nomRepertoireLocal, "/") != NULL
            || estPrefixe(repertoireParent, nomFichier) == false) {

        estRepertoireParent = false;
    }
    return estRepertoireParent;       
}

/**
 * Fonction qui verifie si un fichier ou un repertoire se trouve a la
 * racine du systeme de fichiers.
 *
 * @param  nom      Le nom du fichier ou du repertoire.
 * @return          "true" si le fichier ou le repertoire se trouve a
 *                  la racine du systeme de fichiers, "false" sinon.
 */
bool estARacine(char *nom) {

    ++nom;

    if (strpbrk(nom, "/") != NULL) {
        return false;
    }
    return true;
}

/**
 * Fonction qui recherche l'entree d'un i-node dans le fichier d'i-nodes.
 *
 * S'il existe, cree une instance d'i-node contenant les informations du
 * fichier et le renvoie.
 *
 * @param  nomFichier    Le nom du fichier associe a l'i-node recherche. 
 * @return               Un pointeur pointant sur un i-node si le fichier
 *                       existe dans le systeme de fichiers, NULL sinon. 
 */
Inode *trouverInode(char *nomFichier) {

    Inode *inodeRetour;
    FILE *fichierInodes;

    fichierInodes = fopen(NOM_FICHIER_INODES, "rb");

    if (fichierInodes == NULL) {
        afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
    }

    inodeRetour = malloc(sizeof(Inode));
 
    while(!feof(fichierInodes) ) {
        fread(inodeRetour, sizeof(Inode), 1, fichierInodes);
 
        if(strcmp(inodeRetour->nomFichier, nomFichier) == 0) {
            fclose(fichierInodes);
            return inodeRetour;
        }
    }
    free(inodeRetour);               
    fclose(fichierInodes);

    return NULL;
}

/**
 * Fonction qui verifie si un repertoire existe dans le systeme de fichiers.
 *
 * @param  nomRepertoire        Le nom du repertoire.
 * @return                      "true" si le repertoire existe, "false" sinon. 
 */
bool repertoireExiste(char *nomRepertoire) {

    bool repertoireExiste = false;
    char *unNomRep;
    FILE *fichierRep;

    unNomRep = malloc(TAILLE_MAX_NOM_FICHIER);
    fichierRep = fopen(NOM_FICHIER_REP, "rb");

    while(!feof(fichierRep) && !repertoireExiste) {
  
        fread(unNomRep, TAILLE_MAX_NOM_FICHIER, 1, fichierRep);
        if (strcmp(unNomRep, nomRepertoire) == 0) {
            repertoireExiste = true;
        }
    }
    fclose(fichierRep);
    free(unNomRep);
    return repertoireExiste;
}

/**
 * Fonction qui verifie si le repertoire parent direct d'un fichier ou
 * d'un repertoire existe dans le systeme de fichier.
 *
 * @param  nomFichier       Le nom du fichier ou du repertoire dont on veut
 *                          connaitre l'existence ou non du repertoire parent.
 * @return                  "true" si le repertoire parent direct de nomFichier
 *                          existe dans le systeme de fichiers, "false" sinon. 
 */
bool repertoireParentExiste(char *nomFichier) {

    bool repertoireParentExiste = false;
    FILE *fichierRep;
    char *unNomRep;
    
    if (estARacine(nomFichier) == true) {
        repertoireParentExiste = true;
    }
    else {

        unNomRep = malloc(TAILLE_MAX_NOM_FICHIER);
        fichierRep = fopen(NOM_FICHIER_REP, "rb");

        if (fichierRep == NULL) {
            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }
        while(!feof(fichierRep) && !repertoireParentExiste) {
            fread(unNomRep, TAILLE_MAX_NOM_FICHIER, 1, fichierRep);
            if (estRepertoireParent(unNomRep, nomFichier)) {
                repertoireParentExiste = true;
            }
        }
        free (unNomRep);     
        fclose(fichierRep);
    }
    return repertoireParentExiste;
}

/**
 * Fonction qui verifie si le systeme de fichiers contient suffisamment de 
 * blocs libres pour accomoder un certain nombre d'octets a ecrire. 
 *
 * @param   nbOcetsAPlacer      Le nombre d'ocets a ecrire sur le disque.
 * @return                      "true" s'il y a assez de place sur le disque
 *                              pour ecrire le nombre d'octets demandes, 
 *                              "false" sinon.
 */
bool espaceSuffisant(int nbOctetsAPlacer) {
    int i;
    int nbBlocsLibres = 0;
    bool bloc;
    bool espaceSuffisant = false;
    FILE* fichierBlocsLibres;
        
    fichierBlocsLibres = fopen(NOM_FICHIER_BLOCS_LIBRES, "rb");

    if (fichierBlocsLibres == NULL) {
        afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
    }
    for (i = 0 ; i < NB_MAX_BLOCS && !espaceSuffisant ; ++i) {

        fread(&bloc, sizeof(bool), 1, fichierBlocsLibres);
        
        if (bloc == true) {
            ++nbBlocsLibres;
        }
        if (nbBlocsLibres * TAILLE_BLOC >= nbOctetsAPlacer) {
            espaceSuffisant = true;
        }
    }
    fclose(fichierBlocsLibres);

    return espaceSuffisant;
}
/**
 * Fonction qui valide les arguments d'une commande "creation_fichier".
 *
 * Pour que la creation soit valide :
 * 1 - le fichier ne doit pas etre vide.
 * 2 - le nombre de caracteres du chemin absolue du fichier ne doit pas 
 *     depasser TAILLE_MAX_NOM_FICHIER.
 * 3 - Le nombre d'octets de donnees ne doit pas depasser TAILLE_MAX_FICHIER.
 * 4 - Le repertoire parent doit exister.
 * 5 - Un fichier ayant le meme nom/chemin absolue ne doit pas deja exister.
 * 6 - Le fichier disque doit compter suffisamment de blocs libres pour 
 *     accomoder les donnees du fichier. 
 * La fonction appelle "afficherMessageErreur" si les arguments sont
 * invalides. 
 *
 * @param  nomFichier      Le nom du fichier a creer.
 * @param  donnees         Les donnees du fichier.
 * @return                 "true" si le nom du fichier et les donnees
                           respectent les contraintes du systeme de fichiers,
                           "false" sinon.
 */
bool creationFichierOK (char *nomFichier, char *donnees) {

	bool ok = false;
    int nbOctetsAPlacer;
    Inode *inodeFichier;

    nbOctetsAPlacer = strlen(donnees);

    // si la taille du fichier est si grand qu'il requiere un bloc 
    // d'indirection, ajouter TAILLE_BLOC a nbOctetsAPlacer)
    if (nbOctetsAPlacer > TAILLE_BLOC * NB_BLOCS_DIRECTS) {
        nbOctetsAPlacer = nbOctetsAPlacer + TAILLE_BLOC;
    }
    inodeFichier = trouverInode(nomFichier);

    // message d'erreur si le fichier a ecrire est vide
    if (strcmp(donnees, "\0") == 0) {
        afficherMessageErreur(ERREUR_FICHIER_VIDE, nomFichier);
    }
    // message d'erreur si le chemin absolue du fichier est trop long
    else if (strlen(nomFichier) > TAILLE_MAX_NOM_FICHIER) {
        afficherMessageErreur(ERREUR_TAILLE_NOM_FICHIER, nomFichier);
    }  
    // message d'erreur si la taille du fichier est trop grande.
    else if (strlen(donnees) > TAILLE_MAX_FICHIER) {
        afficherMessageErreur(ERREUR_TAILLE_FICHIER, nomFichier);
    }
    // message d'erreur si le repertoire parent n'existe pas.
    else if (nomFichier[0] != '/' || repertoireParentExiste(nomFichier) == false) {
        afficherMessageErreur(ERREUR_FICHIER_PARENT, nomFichier);
    }
    // message d'erreur si le fichier existe deja. 
    else if (inodeFichier != NULL) {
        afficherMessageErreur(ERREUR_FICHIER_EXISTE, nomFichier);
        free(inodeFichier);
    }
    // message d'erreur si le disque n'a pas assez d'espace. 
    else if (espaceSuffisant(nbOctetsAPlacer) == false) {
        afficherMessageErreur(ERREUR_DISQUE_PLEIN, nomFichier);
    }
    else {
         ok = true;
    }
    return ok;
}

/**
 * Fonction qui ajoute un fichier valide au systeme de fichiers.
 *
 * La fonction modifie :
 * 1 - le fichiers du disque, en y inserant les donnees dans les premiers
 *     blocs disponibles, en fragmentant le fichier au besoin.
 * 2 - le fichier d'i-nodes, en ajoutant une entree d'i-node pour le fichier
 *     nouvellement cree. L'entree d'i-node est inseree a la premiere entree
 *     "invalide", ou ajoutee a la fin du fichier (une entree invalide est une
 *     entree ayant pour valeur de variable nomFichier = "\0", qui signifie
 *     une entree d'i-node qui a ete supprimee via une commande 
 *     "suppression_fichier").
 * 3 - le fichier de blocs libres, en modifiant la valeur des blocs utilises
 *     pour enregistrer les donnees du fichier de "true" a "false". 
 *
 * @param  nomFichier     Le nom du fichier a creer.
 * @param  donnees        Les donnees du fichier a creer.
 * @return
 */
void creationFichier(char *nomFichier, char *donnees) {
    
    int i = 0;
    int noBloc = 0;
    int noPointeur = 0;
    int nbOctetsAPlacer = 0;
    short blocsIndirects[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    char *donneesBloc;
    bool bloc;
    FILE *fichierDisque;
    FILE *fichierInodes;
    FILE *fichierBlocsLibres;
    Inode *inodeFichier;
    Inode *unInode;
 
    if (creationFichierOK(nomFichier, donnees)== true) {
    
        nbOctetsAPlacer = strlen(donnees);
        donnees[strlen(donnees)] = '\0';
        inodeFichier = nouveauInode(nomFichier);

        fichierBlocsLibres = fopen(NOM_FICHIER_BLOCS_LIBRES, "rb+");
        fichierDisque      = fopen(NOM_FICHIER_DISQUE,       "rb+");
        fichierInodes      = fopen(NOM_FICHIER_INODES,       "rb+");

        if(fichierBlocsLibres == NULL || fichierDisque == NULL
                                      || fichierInodes == NULL) {

            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }

        // Tant qu'il reste des blocs a ecrire
        for(noBloc = 0 ; nbOctetsAPlacer > 0 ; ++noBloc) {

            fseek(fichierBlocsLibres, noBloc, SEEK_SET);  
            fread(&bloc, sizeof(bool), 1, fichierBlocsLibres);
            
            if (bloc == true) {
                
                bloc = false;
                fseek(fichierBlocsLibres, -1 * sizeof(bool), SEEK_CUR);
                fwrite(&bloc, sizeof(bool), 1, fichierBlocsLibres);
                
                // si on a ecrit les 8 premier blocs, conserver l'adresse du 
                // prochain bloc libre en memoire - sera le bloc d'indirection
                if (inodeFichier->noBloc[NB_BLOCS_DIRECTS - 1] > 0 && inodeFichier->noBlocIndirection < 0) {

                    inodeFichier->noBlocIndirection = noBloc;
                }
                else {
                    // si on ecrit les blocs directement, conserver les numeros
                    // de bloc dans les i-nodes
                    if (noPointeur < NB_BLOCS_DIRECTS) {
                        inodeFichier->noBloc[noPointeur++] = noBloc;
                    }
                    // si on ecrit les numeros de blocs indirectement, conserver
                    // les numeros de bloc dans un tableau. 
                    else {
                        blocsIndirects[i++] = noBloc;   
                    }

                    // ecriture du bloc de donnees                       
                    donneesBloc = malloc(TAILLE_BLOC);
                    strncpy(donneesBloc, donnees, TAILLE_BLOC);
                    fseek(fichierDisque, noBloc * TAILLE_BLOC, SEEK_SET);
                    fwrite(donneesBloc, TAILLE_BLOC, 1, fichierDisque);

                    donnees = donnees + TAILLE_BLOC;
                    nbOctetsAPlacer = nbOctetsAPlacer - TAILLE_BLOC;
                    free(donneesBloc);
                }
            } 
        }
        // si on a ecrit des blocs indirectement, ecrire les numeros de bloc
        // indirects dans le bloc d'indirection.
        if (inodeFichier->noBlocIndirection > 0) {
            fseek(fichierDisque, inodeFichier->noBlocIndirection * TAILLE_BLOC, SEEK_SET);
            for (i = 0 ; i < 8 ; ++i) {
                fwrite(&blocsIndirects[i], sizeof(short), 1, fichierDisque);
            }
        }
        fclose(fichierDisque);
        fclose(fichierBlocsLibres);

        // ajouter i-node au fichier binaire
        unInode = malloc(sizeof(Inode));
   
        while(!feof(fichierInodes)) {
            fread(unInode, sizeof(Inode), 1, fichierInodes);

            if (strcmp(unInode->nomFichier, "\0") == 0) {
                fseek(fichierInodes, -1 * sizeof(Inode), SEEK_CUR); 
                break;
            }
        }
        fwrite(inodeFichier, sizeof(Inode), 1, fichierInodes); 
        free(unInode);
        free(inodeFichier);
        fclose(fichierInodes);
    }  
}

/**
 * Fonction qui supprime un fichier du systeme de fichiers. 
 *
 * Si le fichier n'existe pas, la fonction appelle "afficherMessageErreur".
 * Sinon, la fonction modifie :
 * 1 - Le fichier du disque, en remplacant les blocs du fichier supprime par
 *     des blocs vides.
 * 2 - Le fichier d'i-nodes, en remplacant l'entree de l'i-node du fichier 
 *     par une entree d'i-node ayant une chaine vide comme valeur d'argument
 *     "nomFichier".
 * 3 - Le fichier de blocs libres, en remplacant de "false" a "true" les valeurs
 *     des entrees associees aux blocs du fichier supprime.
 *
 * @param  nomFichier       Le nom du fichier a supprimer.
 * @return
 */
void suppressionFichier(char *nomFichier) {

    int i;
    short noBlocsIndirects[8];
    bool vrai = true;
    Inode *inodeFichier;
    Inode *inodeVide;

    FILE *fichierDisque;
    FILE *fichierInodes;
    FILE *fichierBlocsLibres;

    inodeVide = nouveauInode((char *) NOM_FICHIER_VIDE);
    inodeFichier = trouverInode(nomFichier);

    if (inodeFichier == NULL) {
        afficherMessageErreur(ERREUR_FICHIER_EXISTE_PAS, nomFichier);
    }
    else {
        fichierDisque      = fopen(NOM_FICHIER_DISQUE,       "rb+");
        fichierInodes      = fopen(NOM_FICHIER_INODES,       "rb+");
        fichierBlocsLibres = fopen(NOM_FICHIER_BLOCS_LIBRES, "rb+");    

        if (fichierDisque == NULL || fichierInodes == NULL
                                  || fichierBlocsLibres == NULL) {

            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }

        // suppression des blocs directs
        for (i = 0 ; i < 8 && inodeFichier->noBloc[i] >= 0 ; ++i) {
            fseek(fichierDisque, inodeFichier->noBloc[i] * TAILLE_BLOC, SEEK_SET);
            fwrite((char *) BLOC_VIDE, TAILLE_BLOC, 1, fichierDisque);

            fseek(fichierBlocsLibres, inodeFichier->noBloc[i], SEEK_SET);
            fwrite(&vrai, sizeof(bool), 1, fichierBlocsLibres);
        }
        // s'il y a bloc d'indirection
        if (inodeFichier->noBlocIndirection != -1) {
            
            // recuperer les numeros de blocs indirects 
            fseek(fichierDisque, inodeFichier->noBlocIndirection * TAILLE_BLOC, SEEK_SET);
            for (i = 0 ; i < 8 ; ++i) {
                fread(&noBlocsIndirects[i], sizeof(short), 1, fichierDisque);
            }
            // suppression des blocs indirects
            for (i = 0 ; noBlocsIndirects[i] != -1 ; ++i) {
                fseek(fichierDisque, noBlocsIndirects[i] * TAILLE_BLOC, SEEK_SET);
                fwrite((char *) BLOC_VIDE, TAILLE_BLOC, 1, fichierDisque);

                fseek(fichierBlocsLibres, noBlocsIndirects[i], SEEK_SET);
                fwrite(&vrai, sizeof(bool), 1, fichierBlocsLibres);
            }
            // suppression du bloc d'indirection
            fseek(fichierDisque, inodeFichier->noBlocIndirection * TAILLE_BLOC, SEEK_SET);
            fwrite((char *) BLOC_VIDE, TAILLE_BLOC, 1, fichierDisque);

            fseek(fichierBlocsLibres, inodeFichier->noBlocIndirection * TAILLE_BLOC, SEEK_SET);
            fwrite(&vrai, sizeof(bool), 1, fichierBlocsLibres);
        }
        // suppression de l'i-node
        while(!feof(fichierInodes)) {
            fread(inodeFichier, sizeof(Inode), 1, fichierInodes);
            if (strcmp(inodeFichier->nomFichier, nomFichier) == 0) {
                fseek(fichierInodes, -1 * sizeof(Inode), SEEK_CUR);
                fwrite(inodeVide, sizeof(Inode), 1, fichierInodes);
                break;
            }
        }
        fclose(fichierDisque);
        fclose(fichierInodes);
        fclose(fichierBlocsLibres);
        free(inodeFichier);
    }
    free(inodeVide);
}

/**
 * Fonction qui valide les arguments d'une commande "creation_repertoire".
 *
 * Pour etre valide :
 * 1 - le chemin absolue doit etre plus petit ou egal a 
 *     TAILLE_MAX_NOM_FICHIER - 2 (doit laisser au minimum deux caracteres : 
 *     1 barre oblique et 1 lettre pour les noms des fichiers qui y seront places).
 * 2 - le repertoire parent direct doit exister dans le systeme de fichier.
 * 3 - le repertoire ne doit pas deja exister.
 *
 * @param  nomRepertoire        Le nom  (chemin absolue) du repertoire dont on 
 *                              veut valider la creation.
 * @return                      "true" si la creation du repertoire est valide.
 *                              "false" sinon.
 */
bool creationRepertoireOK(char *nomRepertoire) {

    bool ok = false;

    // si le chemin absolue du repertoire est trop grand, message d'erreur
    if (strlen(nomRepertoire) > TAILLE_MAX_NOM_FICHIER - 2) {
        afficherMessageErreur(ERREUR_TAILLE_NOM_REP, nomRepertoire);
    } 
    // si le repertoire parent n'existe pas, message d'erreur
    else if (nomRepertoire[0] != '/' || repertoireParentExiste(nomRepertoire) == false) {
        afficherMessageErreur(ERREUR_REP_PARENT, nomRepertoire);
    }
    // si le repertoire existe deja, message d'erreur
    else if(repertoireExiste(nomRepertoire) == true) {
        afficherMessageErreur(ERREUR_REP_EXISTE_DEJA, nomRepertoire);
    }
    else {
        ok = true;
    }
    return ok;

}

/**
 * Fonction qui cree un repertoire valide dans le systeme de fichiers.
 *
 * La fonction modifie le fichier des repertoires, en inserant une entree
 * (une chaine de TAILLE_MAX_NOM_FICHIER caracteres, soit le chemin absolue
 * du repertoire (nomRepertoire)). 
 *
 * @param  nomRepertoire        Le nom (chemin absolue) du repertoire a creer.
 * @return
 */
void creationRepertoire(char *nomRepertoire) {
  
    int i;
    char caractereNull = '\0';
    FILE *fichierRep;
    char *unNomRep;
   
    if (creationRepertoireOK(nomRepertoire) == true) {
        
        unNomRep = malloc(TAILLE_MAX_NOM_FICHIER);
        fichierRep = fopen(NOM_FICHIER_REP, "rb+");

        if (fichierRep == NULL) {
            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }
        while (!feof(fichierRep)) {
            fread(unNomRep, TAILLE_MAX_NOM_FICHIER, 1, fichierRep);
            if (strcmp(unNomRep, (char *) NOM_FICHIER_VIDE) == 0) {
                fseek(fichierRep, -1 * TAILLE_MAX_NOM_FICHIER, SEEK_CUR);
                break;
            }
        }
        fwrite(nomRepertoire, strlen(nomRepertoire), 1, fichierRep);
        i = TAILLE_MAX_NOM_FICHIER - strlen(nomRepertoire);

        for( ; i > 0 ; --i) {
            fwrite(&caractereNull, sizeof(char), 1, fichierRep);
        }
        fclose(fichierRep); 
        free(unNomRep);
    }
    
}

/**
 * Fonction qui supprime un repertoire du systeme de fichiers. 
 *
 * La fonction appelle "afficherMessageErreur" si le repertoire n'est pas
 * present dans le systeme de fichiers, "suppressinFichier" si elle 
 * contient des fichiers, et "suppressionRepertoire" (appel recursif) si
 * elle contient des repertoires.
 *
 * @param  nomRepertoire        Le nom (chemin absolue) durepertoire a 
 *                              supprimer.
 * @return
 */
void suppressionRepertoire(char *nomRepertoire) {
    int i;   
    char *unNomRep; 
    Inode *unInode;
    FILE *fichierRep;
    FILE *fichierInodes;

    if (repertoireExiste(nomRepertoire) == false) {
        afficherMessageErreur(ERREUR_REP_EXISTE_PAS, nomRepertoire);
    }
    else {
        fichierRep    = fopen(NOM_FICHIER_REP,    "rb+");     
        
        if (fichierRep == NULL) {
            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }
        // supprimer le repertoire et les sous-repertoires
        for (i = 0 ; !feof(fichierRep) ; ++i) {
            unNomRep = malloc(TAILLE_MAX_NOM_FICHIER);
            fread(unNomRep, TAILLE_MAX_NOM_FICHIER, 1, fichierRep);

            if (strcmp(nomRepertoire, unNomRep) == 0) {
                fseek(fichierRep, -1 * TAILLE_MAX_NOM_FICHIER, SEEK_CUR);
                fwrite((char *) NOM_FICHIER_VIDE, TAILLE_MAX_NOM_FICHIER, 1, fichierRep);
            }
            else if (estRepertoireParent(nomRepertoire, unNomRep)) {
                fclose(fichierRep);
                suppressionRepertoire(unNomRep);
                fichierRep = fopen(NOM_FICHIER_REP, "rb+");
                fseek(fichierRep, i * TAILLE_MAX_NOM_FICHIER, SEEK_SET);
            } 
            free(unNomRep);
        }
        fichierInodes = fopen(NOM_FICHIER_INODES, "rb");

        if (fichierInodes == NULL) {
            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }
        // supprimer les sous-fichiers
        for (i = 0 ; !feof(fichierInodes) ; ++i) {
            unInode = malloc(sizeof(Inode));
            fread(unInode, sizeof(Inode), 1, fichierInodes);
            if (estRepertoireParent(nomRepertoire, unInode->nomFichier) == true) {
                suppressionFichier(unInode->nomFichier);
            }
            free(unInode);
        }
        fclose(fichierRep);
        fclose(fichierInodes);
    }
}

/**
 * Fonction qui affiche les donnees d'un fichier sur stdout.
 *
 * La fonction appelle "afficherMessageErreur" si le fichier n'existe pas
 * dans le systeme de fichiers.
 *
 * @param  nomFichier       Le nom (chemin absolue) du fichier aux donnees a 
 *                          afficher.
 * @return
 */
void lireFichier(char *nomFichier) {
    
    int i = 0;
    short noBlocsIndirects[8];
    char *donneesBloc;
    Inode *inodeFichier;
    FILE *fichierDisque;
    
    inodeFichier = trouverInode(nomFichier);
    
    // si le fichier n'existe pas, message d'erreur.
    if (inodeFichier == NULL) {
        afficherMessageErreur(ERREUR_LECTURE, nomFichier);
    }
    else {
        fichierDisque = fopen(NOM_FICHIER_DISQUE, "rb");

        if (fichierDisque == NULL) {
            afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
        }
        donneesBloc = malloc(TAILLE_BLOC);
        
       // lecture des blocs directs
        for (i = 0 ; i < 8 && inodeFichier->noBloc[i] >= 0 ; ++i) {
            fseek(fichierDisque, inodeFichier->noBloc[i] * TAILLE_BLOC, SEEK_SET);
            fread(donneesBloc, TAILLE_BLOC, 1, fichierDisque);
            fwrite(donneesBloc, TAILLE_BLOC, 1, stdout);
        } 
        // s'il y a bloc d'indirection
        if (inodeFichier->noBlocIndirection > 0) {
            fseek(fichierDisque, inodeFichier->noBlocIndirection * TAILLE_BLOC, SEEK_SET);

            // recuperer les numero de blocs indirects
            for (i = 0 ; i < 8 ;  ++i) {
                fread(&noBlocsIndirects[i], sizeof(short), 1, fichierDisque);
            }
            // lecture des blocs indirects
            for (i = 0 ; i < 8 && noBlocsIndirects[i] != -1 ; ++i) {
                fseek(fichierDisque, noBlocsIndirects[i] * TAILLE_BLOC, SEEK_SET);
                fread(donneesBloc, TAILLE_BLOC, 1, fichierDisque);
                fwrite(donneesBloc, TAILLE_BLOC, 1, stdout);
            }
        } 
        fprintf(stdout, "\n");   
        fclose(fichierDisque);
        free(inodeFichier);  
    }
    free(donneesBloc);
}

/**
 * Fonction qui lit les lignes de commandes d'un fichier de commandes et 
 * appelle les fonctions appropries selon les commandees et les arguments 
 * entres. 
 *
 * @param  nomFichierCommandes     Le nom du fichier de commandes
 *                                 prealablement valide par la fonction
 *                                 "validerArguments".
 * @return
 */
void executerCommandes(char *nomFichierCommandes) {

    char *nomCommande;
    char *argument1;
    char *argument2; 
    FILE *fichierCommandes;

    fichierCommandes = fopen(nomFichierCommandes, "r");
    
    if (fichierCommandes == NULL) {
        afficherMessageErreur(ERREUR_OUVERTURE_FICHIER, NULL);
    }
    while (!feof(fichierCommandes)) {
       
        nomCommande = malloc(TAILLE_MAX_CHAINE);
        argument1   = malloc(TAILLE_MAX_CHAINE);
        argument2   = malloc(TAILLE_MAX_CHAINE); 

        fscanf(fichierCommandes, "%s", nomCommande);

        if (strcmp(nomCommande, "creation_repertoire") == 0) {
            fscanf(fichierCommandes, "%s",  argument1);
            creationRepertoire(argument1); 
        }
        else if (strcmp(nomCommande, "suppression_repertoire") == 0) {
            fscanf(fichierCommandes, "%s", argument1);
            suppressionRepertoire(argument1);
        }
        else if (strcmp(nomCommande, "creation_fichier") == 0) {
            fscanf(fichierCommandes, "%s %[^\n]", argument1, argument2);
            if (argument2[strlen(argument2) - 1] == WINDOWS_FIN_LIGNE) {
                argument2[strlen(argument2) - 1] = '\0';
            }
            creationFichier(argument1, argument2);
        }
        else if (strcmp(nomCommande, "suppression_fichier") == 0) {
            fscanf(fichierCommandes, "%s", argument1);
            suppressionFichier(argument1);
        }
        else if (strcmp(nomCommande, "lire_fichier") == 0) {
            fscanf(fichierCommandes, "%s", argument1);
            lireFichier(argument1); 
        }
        free(nomCommande);
        free(argument1);
        free(argument2);
    }
    fclose(fichierCommandes);
}

/**
 *  Main
 */
int main (int argc, char *argv[]) {
       
    validerArguments(argc, argv);      
    verifierFichierDisque();
    executerCommandes(argv[1]);  
    return 0;     
}
