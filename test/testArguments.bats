#!/usr/bin/env bats

@test "0 argument" {
    run ./fs
    [ "${lines[0]}" = "Erreur, nombre d'arguments invalide." ]
    [ "${lines[1]}" = "Utilisation : ./fs nom_fichier" ]
}

@test "2 arguments" {
    run ./fs fichier.txt argument2
    [ "${lines[0]}" = "Erreur, nombre d'arguments invalide." ]
    [ "${lines[1]}" = "Utilisation : ./fs nom_fichier" ]
}

@test "Fichier en argument n'existe pas" {
    run ./fs fichierBidon.txt
    [ "$output" = "Erreur, fichier en argument \"fichierBidon.txt\" non-existant." ]
}

