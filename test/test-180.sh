#!/bin/sh

#
# Quelques tests pour vérifier les options/paramètres
#

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

# tests élémentaires sur les options

./detecter -i texte -l 1      ls -l				&& fail "-i texte"
./detecter -i 1     -l texte  ls -l				&& fail "-l texte"
./detecter -i texte -l texte  ls -l				&& fail "-i texte -l texte"
./detecter -i 1     -l 1 	  programmeInconnu 	|| fail "Programme inconnu"
./detecter -i 1 	-l -c     ls -l  			&& fail "-l -c; manque un arg"

# vérifie si le programme détecte un changement dans un fichier 
# qui ne change jamais

echo 'toto' > ./toto.tmp
toto=$(./detecter -i 1     -l 2      'cat' 'toto.tmp' | grep -c 'toto')
if [ $toto -gt 1 ]
then
	fail "la comparaison ne marche pas"
fi
rm ./toto.tmp

exit 0