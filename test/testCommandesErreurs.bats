# !/usr/bin/env bats

@test "Creation repertoire existe deja" {
    run ./fs test/commandesErreurs1.txt
    [ "$output" = "Erreur, creation du repertoire \"/repertoire1\" impossible : le repertoire existe deja." ]
}

@test "Creation repertoire dont le repertoire parent n'existe pas" {
    run ./fs test/commandesErreurs2.txt
    [ "$output" = "Erreur, creation du repertoire \"/repertoireBidon/repertoire2\" impossible : le repertoire parent n'existe pas." ]
}

@test "Creation repertoire dont le chemin absolu est plus grand que 38 caracteres (cas limite : 39 caracteres)" {
    run ./fs test/commandesErreurs3.txt
    [ "$output" = "Erreur, creation du repertoire \"/23456789012345678901234567890123456789\" impossible : le chemin absolue depasse 38 caracteres." ]
}

@test "Creation repertoire dont le chemin absolu est plus grand que 38 caracteres (cas general : 50 caracteres)" {
    run ./fs test/commandesErreurs4.txt
    [ "$output" = "Erreur, creation du repertoire \"/2345678901234567890123456789012345678901234567890\" impossible : le chemin absolue depasse 38 caracteres." ]
}

@test "Suppression repertoire qui n'existe pas" {
    run ./fs test/commandesErreurs5.txt
    [ "$output" = "Erreur, suppression du repertoire \"/repertoireBidon\" impossible : le repertoire n'existe pas." ]
}

@test "Creation fichier vide" {
    run ./fs test/commandesErreurs6.txt
    [ "$output" = "Erreur, creation du fichier \"/fichierVide\" impossible : le fichier est vide." ]
}

@test "Creation fichier qui existe deja" {
    run ./fs test/commandesErreurs7.txt
    [ "$output" = "Erreur, creation du fichier \"/fichierExisteDeja\" impossible : le fichier existe deja." ]
}

@test "Creation fichier repertoire n'existe pas" {
    run ./fs test/commandesErreurs8.txt
    [ "$output" = "Erreur, creation du fichier \"/repertoireBidon/fichier\" impossible : le repertoire parent n'existe pas." ]
}
@test "Creation fichier plus grand que 256 octets (cas limite : 257 octets)" {
    run ./fs test/commandesErreurs9.txt
    [ "$output" = "Erreur, creation du fichier \"/fichierVolumineux\" impossible : la taille du fichier depasse 256 octets." ]
}

@test "Creation fichier plus grand que 256 octets (cas general : 500 octets)" {
    run ./fs test/commandesErreurs10.txt
    [ "$output" = "Erreur, creation du fichier \"/fichierVolumineux\" impossible : la taille du fichier depasse 256 octets." ]
}

@test "Creation fichier dont le chemin absolue est plus grand que 40 caracteres (cas limite : 41 caracteres)" {
    run ./fs test/commandesErreurs11.txt
    [ "$output" = "Erreur, creation du fichier \"/2345678901234567890123456789012345678901\" impossible : le chemin absolue depasse 40 caracteres." ]
}

@test "Creation fichier dont le chemin absolue est plus grand que 40 caracteres (cas general : 50 caracteres)" { 
    run ./fs test/commandesErreurs12.txt
    [ "$output" = "Erreur, creation du fichier \"/2345678901234567890123456789012345678901234567890\" impossible : le chemin absolue depasse 40 caracteres." ]
}

@test "Creation fichier disque plein" {
    cp test/fsPlein/disque.dat disque.dat
    cp test/fsPlein/blocs_libres.dat blocs_libres.dat
    cp test/fsPlein/repertoires.dat repertoires.dat
    cp test/fsPlein/inodes.dat inodes.dat
    run ./fs test/commandesErreurs13.txt
    [ "$output" = "Erreur, creation du fichier \"/fichierDisquePlein\" impossible : la taille du fichier est plus grande que l'espace disque restante." ]
}

@test "Suppression fichier n'existe pas" {
    run ./fs test/commandesErreurs14.txt
    [ "$output" = "Erreur, suppression du fichier \"/fichierBidon\" impossible : le fichier n'existe pas." ]
}

@test "Lecture fichier n'existe pas" {
    run ./fs test/commandesErreurs15.txt
    [ "$output" = "Erreur, lecture du fichier \"/fichierBidon\" impossible : le fichier n'existe pas." ]
}
