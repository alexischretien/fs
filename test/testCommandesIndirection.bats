#!/usr/bin/env bats

@test "Ecriture et lecture fichier de 129 octets (cas limite, 9 blocs + 1 bloc indirection)" {
    run ./fs test/commandesIndirection1.txt;
    [ "$output" = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" ]
}

@test "Ecriture et lecture fichier de 200 octets (cas general, 13 blocs + 1 bloc indirection)" {
    run ./fs test/commandesIndirection2.txt
    [ "$output" = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002" ]
}

@test "Ecriture et lecture fichier de 256 octets (cas limite, 16 blocs + 1 bloc indirection)" {
    run ./fs test/commandesIndirection3.txt
    [ "$output" = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003" ]
}

@test "Ecriture et lecture fichier de 256 octets (16 blocs + 1 bloc indirection) fragmente" {
    run ./fs test/commandesIndirection4.txt;
    [ "$output" = "bloc1***********bloc2***********bloc3***********bloc4***********bloc5***********bloc6***********bloc7***********bloc8***********bloc9***********bloc10**********bloc11**********bloc12**********bloc13**********bloc14**********bloc15**********bloc16**********" ]
}
