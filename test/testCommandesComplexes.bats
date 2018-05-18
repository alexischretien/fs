#usr/bin/env bats

@test "Creation repertoires et sous-repertoires, creations fichiers, lecture fichiers" {
    run ./fs test/commandesComplexes1.txt
    [ "${lines[0]}" = "donnees fichier racine 1" ]
    [ "${lines[0]}" = "donnees fichier racine 1" ]
    [ "${lines[1]}" = "donnees fichier racine 2" ]
    [ "${lines[2]}" = "donnees fichier publique" ]
    [ "${lines[3]}" = "0101010111010001010111010001" ]
    [ "${lines[4]}" = "donnees fichier sarah" ]
    [ "${lines[5]}" = "donnees fichier christian tres tres long fichier dans le but de tester la lecture et la suppression d'un fichier qui inclut un bloc d'indirection blablabla 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20" ]
    [ "${lines[6]}" = "donnees fichier dominique" ]
    [ "${lines[7]}" = "pleins de pixels qui forment une image" ]
    [ "${lines[8]}" = "un autre tas de pixels" ]
    [ "${lines[9]}" = "E Am C E C Am C E D" ]
    [ "${lines[10]}" = "something wildcard(*.c) something" ]
    [ "${lines[11]}" = "if{} else if{} else{}" ]
}

@test "Suppression de fichiers, tentative de lecture des fichiers" {
    run ./fs test/commandesComplexes2.txt
    echo "status = ${status}"
    echo "output = ${output}"
    [ "${lines[3]}" = "donnees fichier racine 1" ]
    [ "${lines[4]}" = "donnees fichier racine 2" ]
    [ "${lines[5]}" = "donnees fichier publique" ]
    [ "${lines[0]}" = "Erreur, lecture du fichier \"/os/fichier.o\" impossible : le fichier n'existe pas." ]
    [ "${lines[6]}" = "donnees fichier sarah" ]
    [ "${lines[1]}" = "Erreur, lecture du fichier \"/user/christian/fichierChristian\" impossible : le fichier n'existe pas." ]
    [ "${lines[2]}" = "Erreur, lecture du fichier \"/user/dominique/fichierDominique\" impossible : le fichier n'existe pas." ]
    [ "${lines[7]}" = "pleins de pixels qui forment une image" ]
    [ "${lines[8]}" = "un autre tas de pixels" ]
    [ "${lines[9]}" = "E Am C E C Am C E D" ]
    [ "${lines[10]}" = "something wildcard(*.c) something" ]
    [ "${lines[11]}" = "if{} else if{} else{}" ]
}

@test "Ecriture et lecture d'un fichier portant le meme nom qu'un fichier qui a ete supprime" {
     run ./fs test/commandesComplexes3.txt
     [ "${lines[2]}" = "donnees fichier racine 1" ]
     [ "${lines[3]}" = "donnees fichier racine 2" ]
     [ "${lines[4]}" = "donnees fichier publique" ]
     [ "${lines[0]}" = "Erreur, lecture du fichier \"/os/fichier.o\" impossible : le fichier n'existe pas." ]
     [ "${lines[5]}" = "donnees fichier sarah" ]
     [ "${lines[6]}" = "le fichier christian a ete recree apres avoir ete supprime. le fait qu'il porte le meme nom que l'ancien fichier ne cause aucun probleme." ]
     [ "${lines[1]}" = "Erreur, lecture du fichier \"/user/dominique/fichierDominique\" impossible : le fichier n'existe pas." ]
     [ "${lines[7]}" = "pleins de pixels qui forment une image" ]
     [ "${lines[8]}" = "un autre tas de pixels" ]
     [ "${lines[9]}" = "E Am C E C Am C E D" ]
     [ "${lines[10]}" = "something wildcard(*.c) something" ]
     [ "${lines[11]}" = "if{} else if{} else{}" ]
 }

@test "Suppression d'un repertoire entrainant suppression de sous-repertoires et sous-fichiers. Tentative de lecture des fichiers" {
    run ./fs test/commandesComplexes4.txt
    [ "${lines[9]}" = "donnees fichier racine 1" ]
    [ "${lines[10]}" = "donnees fichier racine 2" ]
    [ "${lines[11]}" = "donnees fichier publique" ]
    [ "${lines[0]}" = "Erreur, lecture du fichier \"/os/fichier.o\" impossible : le fichier n'existe pas." ]
    [ "${lines[1]}" = "Erreur, lecture du fichier \"/user/sarah/fichierSarah\" impossible : le fichier n'existe pas." ]
    [ "${lines[2]}" = "Erreur, lecture du fichier \"/user/christian/fichierChristian\" impossible : le fichier n'existe pas." ]
    [ "${lines[3]}" = "Erreur, lecture du fichier \"/user/dominique/fichierDominique\" impossible : le fichier n'existe pas." ]
    [ "${lines[4]}" = "Erreur, lecture du fichier \"/user/sarah/images/image.png\" impossible : le fichier n'existe pas." ]
    [ "${lines[5]}" = "Erreur, lecture du fichier \"/user/sarah/images/image2.png\" impossible : le fichier n'existe pas." ]
    [ "${lines[6]}" = "Erreur, lecture du fichier \"/user/sarah/musique/file.mp3\" impossible : le fichier n'existe pas." ]
    [ "${lines[7]}" = "Erreur, lecture du fichier \"/user/dominique/uni/Makefile\" impossible : le fichier n'existe pas." ]
    [ "${lines[8]}" = "Erreur, lecture du fichier \"/user/dominique/uni/tp1.c\" impossible : le fichier n'existe pas." ]
}
