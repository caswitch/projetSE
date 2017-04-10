# Table des matières

Nicolat Argyriou
Marie-France Kommer

## Sujet

L'objectif de ce projet était de créer une commande ```detecter``` permettant de lancer périodiquement un programme et de détecter les changements dans sa sortie standard en respectant scrupuleusement une spécification.

```
detecter [-t format][-i intervalle][-l limite][-c] prog arg1 arg2 ... argn
```

## Difficultés

Le projet a amené son lot de difficultés auxquelles nous nous attendions,

* Comme c'est souvent le cas pour les travaux de groupe, nous avons du faire preuve de coordination, discuter ensemble de l'architecture que nous choisirions pour notre code, nous séparer les tâches et enfin, coordonner nos prières envers les dieux de git tous les soirs
* Il a fallu s'y reprendre à plusieurs fois pour coder la boucle principale afin de respecter le sujet et la spécification autant que possible

Et quelques unes plus spécifiques

* Comment diviser le problème en sous-problème et les articuler ensemble pour avoir un programme fonctionnel ?
* Comment comparer les sorties entre les différents appels du programme passé en argument ? 
* Comment allons-nous nous débrouiller pour stocker en mémoire ces sorties dont nous ne connaissons pas la taille finale avant d'avoir reçu leur EOF ?
* Comment utiliser les informations liées à la couverture de code et comment augmenter cette couverture ?
* Comment trouver un jeu de tests pertinent pour couvrir le code et tester son bon fonctionnement ?

Pour répondre à ces problèmes, nous avons fait quelques choix. 

## Spécificités de notre implémentation

#### Documentation

Notre projet est entièrement commenté avec une syntaxe compatible avec doxygen. Il est donc possible de générer cette documentation avec la commande ```doxygen``` à exécuter dans la racine du projet.

Nous avons pris soin d'écrire un maximum de choses en anglais avec l'éditeur de texte [SimpleWriter](https://xkcd.com/simplewriter/) qui nous limite aux 10.000 mots les plus communs de la langue anglaise.

#### Méthode

Notre méthode de travail a été de diviser le problème en un maximum de sous-problèmes et de laisser chaque partie du programme s'occuper de son propre morceau de complexité.

Cela nous permet d'avoir un code (nous l'espérons) lisible et de potentiellement pouvoir réimplémenter certains morceaux de notre programme dans de futurs projet faisant face aux mêmes types de difficultés.

#### Appels, redirections

Nous avons crée une fonction ```callProgram``` ([doc]()) se chargeant d'appeler le programme passé en argument avec ses arguments et qui renvoie un tube vers lequel est redirigé la sortie standard du dit programme. Les autres fonctions de notre algorithme utiliseront donc ce tube pour détecter d'éventuels changements dans la sortie de ce programme par rapport à ses autres appels.

[Voir ```callProgram()```](doc/html/detecter_8h.html#a29aec1ddf28d146551c18cce81ed5399)


#### Structures

Pour le stockage et la comparaison, nous avons crée trois structures de données:

* [Une structure ```sFile```](doc/html/structs__file.html), initialisable à partir d'un file descriptor renvoyé par ```open``` et permettant l'utilisation d'un getchar bufferisé sur celui-ci.
* [Une structure ```Buffer```](doc/html/structs__buff.html), une liste doublement chaînée de ```Nodes```. Les buffers nous permettent de stocker un nombre virtuellement illimité d'élements, que nous manipulons grâce un certain nombre de fonctions, notamment un putchar et un getchar
* [Une structure ```Node```](doc/html/structs__node.html), un maillon de liste chaînée. Ce sont des tableaux de N=256 éléments de type unique, mais variable (nous utilisons des char). Ils connaissent localement l'adresse de leur successeur et de leur prédécesseur

[Voir ```l'encapsulation```](doc/html/buff__and__file_8h.html)

#### Lecture, comparaison et stockage du flux de données

Nous avons décidé de cacher la complexité de la comparaison et du stockage des données en utilisant uniquement les fonctions getchar et putchar dans lesquelles le vrai travail en mémoire se fait.

Ces getchars nous permettent de comparer l'ancienne sortie et la nouvelle au fur et à mesure que le programme fils s'exécute, à travers une boucle extrêmement lisible, comparant simplement caractère par caractère les deux flux et remplacant le contenu du buffer au fur et à mesure avec un appel à putchar.

* Nous avons introduit *un* effet de bord (documenté) dans la fonction output_delta que vous risquerez de ne pas aimer. Nous utilisons le même buffer passé en argument pour stocker les nouvelles données, ce qui permet de moins malmener le cache de nos processeurs, de faire moins souvent des allocations et des frees pour les programmes écrivant beaucoup. 
En effet, à tout instant, nous lisons et écrivons dans des zones mémoire très proches.
* Nous avons choisi d'encapsuler entièrement nos structures de donnée afin de nous permettre de les changer sans impacter le programme principal, ce que nous avons d'ailleurs fait: au début nous utilisions un buffer de taille fixe et des reallocs.

[Voir ```output_delta()```](doc/html/detecter_8h.html#a5351354317915a33d5a3c1c2611a5315)

#### Couverture et jeux de tests

Nous avons mis nos jeux de tests additionnels dans test-180.sh

Nous testons plusieurs choses,

- Quelques petites choses sur les arguments du programme
  - L'ordre des options a-t-il une importance ? (Non)
  - La présence ou non de toutes les options a-t-elle une importance ? (NON)
    - Dans certains cas précis, par exemple, ```./detecter -l1 -i1 toto $DN ``` renvoyait EXIT_SUCCESS alors que `` `./detecter -l1 -i1 -c toto $DN``` renvoyait EXIT_FAILURE. Ceci s'expliquait car le processus fils ne signalait pas au père que la commande donnée en argument de execvp était invalide à moins d'avoir l'option -c spécifier. Créer des jeux de tests nous a donc permis de voir des faiblesses d'implémentation et de les corriger.
  - Les options sont-elles valides ?
  - Les arguments des options sont-ils valides ?
  - getopt comprend-il les syntaxes qu'il devrait comprendre ?
- Si il détecte bien quand l'utilisateur a rentré une commande qui n'existe pas
- Si aucun changement n'est détecté entre deux appels d'un programme qui affiche toujours la même chose
- Si le programme se comporte normalement lorsque les données qu'on lui fournit font 256 caractères de long, juste assez pour dépasser d'un cran la taille d'un maillon de sa liste chaînée.

Pour augmenter la couverture (de façon artificielle) du code, nous avons mis nos fonctions de tests dans des MACRO.

* GRUMBLE(msg) : Retourne un message d'erreur et quitte le programme.
* ALLOC_NULL(alloc, msg, ptr, OPERATION) : Test le bon fonctionnement d'un malloc, sinon libère la mémoire qui doit l'être et appelle GRUMBLE
* ASSERT(val, OPERATION) : Test si la valeur est égal à -1 et appelle OPERATION (une instruction, un fonction ou une macro)
* CHECK_NULL(ptr, OPERATION) : Test si le pointeur est NULL. Si oui, appelle OPERATION (une instruction, un fonction ou une MACRO)
* CHECK_ZERO(val, OPERATION) : Test si la valeur est égal à 0 et appelle OPERATION (une instruction, un fonction ou une MACRO)
 
Nous avons intentionnellement gardé plusieurs MACRO qui ont un comportement similaire dans un soucis de lisibilité du code.

## Architecture du programme

Notre programme commence dans sa fonction main dans laquelle il: 

* Initialise ses options (symbolisées par des variables opt_X) à leurs valeurs par défaut
* Analyse les arguments entrés par l'utilisateur et modifie ses options en conséquence
* Potentiellement appelle la fonction usage() si les arguments sont mauvais
* Appelle le coeur du programme, la fonction ```interval()```

[Voir ```Main```]()

L'allocation du [```buffer```](doc/html/structs__buff.html) qui durera pendant l'intégralité du programme se fait ici

Cette fonction appelle N fois le programme X avec les arguments Y toutes les K millisecondes. 

[Voir ```interval()```](doc/html/detecter_8h.html#aa4efd77ab6ae544b8985d542daa63a2f)

L'exécution du programme se fait via la fonction callProgram se chargeant d'appeler le programme passé en argument avec ses arguments et qui renvoie un tube vers lequel est redirigé la sortie standard du dit programme.

[Voir ```callProgram()```](doc/html/detecter_8h.html#a29aec1ddf28d146551c18cce81ed5399)

Ce file descriptor est ensuite passé à la fonction output_delta(), ainsi que le buffer, qui saura nous dire si les données sorties par le programme diffèrent de celles présentes dans le buffer.

Si c'est le cas, la fonction buff_print() se charge d'afficher les données.

[```Voir buff_print()```](doc/html/buff__and__file_8h.html#a8a6915be4ebc290760baff3bc2378b00) 

Dans tous les cas, nous attendons ensuite l'extinction du programme fils invoqué par callProgram() avec la fonction exit_code().

[```Voir exit_code()```](doc/html/detecter_8c.html#aa07b951f949b7b01251f236dd4d62405)
