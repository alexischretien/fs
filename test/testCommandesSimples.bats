#!/usr/bin/env bats

@test "Ecriture et lecture fichier de 1 octet  (1 bloc)" {
    run ./fs test/commandesSimples1.txt
    [ "$output" = "1" ]
}

@test "Ecriture et lecture fichier de 8 octets (1 bloc)" {
    run ./fs test/commandesSimples2.txt
    [ "$output" = "00000002" ]
}

@test "Ecriture et lecture fichier de 16 octets (1 bloc)" {
    run ./fs test/commandesSimples3.txt
    [ "$output" = "0000000000000003" ]   
}

@test "Ecriture et lecture fichier de 17 octets (2 blocs)" {
    run ./fs test/commandesSimples4.txt
    [ "$output" = "00000000000000004" ]
}

@test "Ecriture et lecture fichier de 24 octets (2 blocs)" {
    run ./fs test/commandesSimples5.txt
    [ "$output" = "000000000000000000000005" ]
}

@test "Ecriture et lecture fichier de 32 octets (2 blocs)" {
    run ./fs test/commandesSimples6.txt
    [ "$output" = "00000000000000000000000000000006" ]
}

@test "Ecriture et lecture fichier de 33 octets (3 blocs)" {
    run ./fs test/commandesSimples7.txt
    [ "$output" = "000000000000000000000000000000007" ]
}

@test "Ecriture et lecture fichier de 40 octets (3 blocs)" {
    run ./fs test/commandesSimples8.txt
    [ "$output" = "0000000000000000000000000000000000000008" ]
}

@test "Ecriture et lecture fichier de 48 octets (3 blocs)" {
    run ./fs test/commandesSimples9.txt
    [ "$output" = "000000000000000000000000000000000000000000000009" ]
}

@test "Ecriture et lecture fichier de 128 octets (8 blocs)" {
    run ./fs test/commandesSimples10.txt
    [ "$output" = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010" ]
}
@test "Suppression fichiers, ecriture et lecture fichier de 40 octets (3 blocs) fragmente" {
    run ./fs test/commandesSimples11.txt
    [ "$output" = "0000000000000000000000000000000000000011" ] 
}

@test "Ecriture et lecture d'un fichier dans un repertoire autre que le repertoire racine" {
    run ./fs test/commandesSimples12.txt
    [ "$output" = "00000012" ]
}

@test "Ecriture et lecture d'un fichier contenant le caractere espace dans ses donnees" {
    run ./fs test/commandesSimples13.txt
    [ "$output" = "Ceci est une phrase." ]
}

@test "Creation repertoire dont le chemin absolue est de 38 caracteres (cas limite)" {
    run ./fs test/commandesSimples14.txt
    [ "$output" = "" ]
}

@test "Creation fichier dont le chemin absolue est de 40 caracteres (cas limite)" {
    run ./fs test/commandesSimples15.txt
    [ "$output" = "" ]
}
