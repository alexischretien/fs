# fs

## Description
Programme de systeme de fichiers basé sur les i-nodes. 

Les i-nodes du systeme de fichiers supportent seulement  l'indirection  simple.  
Seul l'i-node associé à un fichier n'est chargé en memoire lors de son ouverture.

Le systeme de fichiers est compose des fichiers suivants :
* Un fichier binaire de disque de 512 Ko (32 000 blocs de 16 octets).
* Un fichier binaire de blocs libres de 32 Ko
* Un fichier binaire enregistrant les donnees des i-nodes.
* Un fichier binaire enregistrant les noms des repertoires. 
## Utilisation
```
./fs [NOM_FICHIER_COMMANDES]
```
`NOM_FICHIER_COMMANDES`  doit exister.  Si Le  fichier  de  disque 
n'est pas  présent dans le repertoire d'execution  du programme, 
le programme génèrera une fichier de  disque vide  et remplacera les
fichiers de  blocs libres,  d'i-nodes  et de répertoires par des 
fichiers vides. Si le fichier  de  disque  existe, le  programme
assumera que  les  fichiers  de  blocs  libres,  d'i-nodes  et  de 
répertoires sont tous cohérents les uns avec les autres. 

Les commandes valides d'un fichier de commandes sont :

* `creation_repertoire NOM_REPERTOIRE`
* `suppression_repertoire NOM_REPERTOIRE`
* `creation_fichier NOM_FICHIER DONNEES`
* `suppression_fichier NOM_FICHIER`
* `lire_fichier NOM_FICHIER`
## Auteur
Alexis Chrétien
