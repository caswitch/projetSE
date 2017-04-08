# Table des matières


# Introduction

### Sujet

L'objectif de ce projet était de créer une commande ```detecter``` permettant de lancer périodiquement un programme et de détecter les changements dans sa sortie standard en respectant scrupuleusement une spécification.

```
detecter [-t format][-i intervalle][-l limite][-c] prog arg1 arg2 ... argn
```

### Difficultés

Le projet a amené son lot de difficultés auxquelles nous nous attendions,

* Comme c'est souvent le cas pour les travaux de groupe, nous avons du faire preuve de coordination, discuter ensemble de l'architecture que nous choisirions pour notre code, nous séparer les tâches et enfin, coordonner nos prières envers les dieux de git tous les soirs
* Il a fallu s'y reprendre à plusieurs fois pour coder la boucle principale afin de respecter le sujet et la spécification autant que possible

Et quelques unes plus spécifiques

* Comment comparer les sorties entre les différents appels du programme passé en argument ? 
* Comment allons-nous nous débrouiller pour stocker en mémoire ces sorties dont nous ne connaissons pas la taille finale avant d'avoir reçu leur EOF ?

Pour répondre à ces problèmes, nous avons fait quelques choix. 

# Corps

### Spécificités de notre implémentation

#### Documentation

Notre projet est entièrement commenté avec une syntaxe compatible avec doxygen. Il est donc possible de générer cette documentation avec la commande ```doxygen``` à exécuter dans la racine du projet.

Nous avons pris soin d'écrire un maximum de choses en anglais avec l'éditeur de texte [SimpleWriter](https://xkcd.com/simplewriter/) qui nous limite aux 10.000 mots les plus communs de la langue anglaise.

#### Méthode

Notre méthode de travail a été de diviser le problème en un maximum de sous-problèmes et de laisser chaque partie du programme s'occuper de son propre morceau de complexité.

Cela nous permet d'avoir un code (nous l'espérons) lisible et de potentiellement pouvoir réimplémenter certains morceaux de notre programme dans de futurs projet faisant face aux mêmes types de difficultés.

#### Appels, redirections

Nous avons crée une fonction ```callProgram``` se chargeant d'appeler le programme passé en argument avec ses arguments et qui renvoie un tube vers lequel est redirigé la sortie standard du dit programme. Les autres fonctions de notre algorithme utiliseront donc ce tube pour détecter d'éventuels changements dans la sortie de ce programme par rapport à ses autres appels.

#### Structures

Pour le stockage et la comparaison, nous avons crée deux structures de données:

* Une structure "Fichier", initialisable à partir d'un file descriptor renvoyé par ```open``` et permettant l'utilisation d'un getchar bufferisé sur celui-ci.
* Une structure "Buffer", permettant de stocker un nombre virtuellement illimité de caractères, à laquelle nous avons associé un certain nombre de fonctions, notamment un putchar et un getchar

#### Lecture, comparaison et stockage du flux de données

Nous avons décidé de cacher la complexité de la comparaison et du stockage des données en utilisant uniquement les getchar et des putchar dans lesquels le vrai travail en mémoire se fait.

Ces getchars nous permettent de comparer l'ancienne sortie et la nouvelle au fur et à mesure que le programme fils s'exécute, à travers une boucle extrêmement lisible, comparant simplement caractère par caractère les deux flux et remplacant le contenu du buffer au fur et à mesure avec un appel à putchar.

* Nous avons introduit *un* effet de bord (documenté) dans la fonction output_delta que vous risquerez de ne pas aimer. Nous utilisons le même buffer à chaque fois, ce qui permet de ne pas malmener le cache de nos processeurs et de faire moins souvent des allocations et des frees pour les programmes écrivant beaucoup. 
* Nous avons choisi d'encapsuler entièrement nos structures de donnée afin de nous permettre de les changer sans impacter le reste du programme
* Nous avons opté pour l'utilisation de reallocs pour redimensionner notre buffers mais avons envisagé une listes chaînée. 
* 
