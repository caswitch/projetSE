#!/bin/sh

TEST=$(basename $0 .sh)

TMP=/tmp/$TEST
LOG=$TEST.log
V=${VALGRIND}		# appeler avec la var. VALGRIND à "" ou "valgrind -q"

exec 2> $LOG
set -x

fail ()
{
    echo "==> Échec du test '$TEST' sur '$1'."
    echo "==> Log : '$LOG'."
    echo "==> Exit"
    exit 1
}

DN=/dev/null

# tests élémentaires supplémentaires sur les options

./detecter -h && fail "option help"

./detecter -r && fail "option inconnue"

./detecter -t && fail "l'option nécessite un argument"

./detecter -i && fail "l'option nécessite un argument"

./detecter -l && fail "l'option nécessite un argument"

./detecter -i1 -l1 -c cat $DN || fail "syntaxe -i1 -l1 -c"

./detecter -t "i" cat $DN && fail "invalid format for -t option"

./detecter -t "format%c" -i 1 -l 1 cat $DN || fail "format for -t option"

./detecter -l texte cat $DN && fail "argument de -l invalide"

./detecter -i texte cat $DN && fail "argument de -i invalide"

./detecter -l1 -i1 toto $DN && fail "cmd inconnue"

./detecter -l1 -i1 -c toto $DN && fail "cmd inconnue"

# test la détection d'un changement dans un fichier

echo 'toto' > ./toto.tmp
toto=$(./detecter -i1 -l2 'cat' 'toto.tmp' | grep -c 'toto')
if [ $toto -gt 1 ]
then
	fail "la comparaison ne marche pas"
fi
rm ./toto.tmp

exit 0
