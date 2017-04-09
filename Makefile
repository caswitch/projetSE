#
# Ce Makefile contient les cibles suivantes :
#
# all		: compile les programmes
# clean		: supprime les fichiers générés automatiquement
# coverage	: compile les programmes pour mesurer la couverture de code
# test		: lance tous les tests (scripts shell test-xxx.sh)
#		  (sans appeler valgrind)
# gcov		: génère les rapports de couverture (à lancer après avoir
#		  lancé les cibles 'coverage' et 'test').
#		  Résultats dans *.gcov
# ctags		: génère un fichier tags pour la navigation avec vim.
#		  (voir http://usevim.com/2013/01/18/tags/)
#
# De plus, les cibles supplémentaires suivantes sont fournies pour
# simplifier les tâches répétitives :
#
# test-avec-valgrind	: lance les tests avec valgrind (conseillé)
# couverture-et-tests	: automatise les tests avec rapport de couverture
#

# Dossier

DIROBJ  = obj
DIRINC  = include
DIRSRC  = src
DIRTEST = test

# Compiler

CC      = gcc
COV     = -coverage
CFLAGS  = -Wall -Wextra -Werror -std=gnu99 -g $(COVERAGE) $(DEBUG)

# Dépendances, sources, objets, exécutable

DEPS    = $(wildcard include/*.h)
SOURCES = $(wildcard src/*.c)
TESTS   = $(wildcard test/*.c)
OBJETS  = $(SOURCES:src/%.c=obj/%.o)
EXEC    = detecter

.PHONY: all
all: ctags $(EXEC)

$(EXEC): $(OBJETS)
	$(CC) $(CFLAGS) -o $@ $^

$(DIROBJ)/%.o: $(DIRSRC)/%.c $(DEPS)
	@mkdir -p $(DIROBJ)
	$(CC) $(CFLAGS) -c -I$(DIRINC) -o $@ $<

# Cibles à appeler manuellement

.PHONY: exec_test
exec_test: $(EXEC)
	./$(EXEC) -c -i 1000 -l 4 -t "%H:%M:%S" echo "test d'exécution"

.PHONY: coverage
coverage: clean
	$(MAKE) COVERAGE=$(COV)

.PHONY: gcov
gcov:
	gcov $(SOURCES) -o $(DIROBJ)

# Par défaut, "test" lance les tests sans valgrind.
# Si on souhaite utiliser valgrind (conseillé), positionner la
# variable VALGRIND ou utiliser la cible "test-avec-valgrind"

.PHONY: test
test: test-sans-valgrind

.PHONY: test-sans-valgrind
test-sans-valgrind: all
	@for i in test/test-*.sh ; do echo $$i ; sh $$i || exit 1 ; done

.PHONY: test-avec-valgrind
test-avec-valgrind: all
	VALGRIND="valgrind" ; export VALGRIND ; for i in test/test-*.sh ; do echo $$i ; sh $$i || exit 1 ; done

.PHONY: couverture-et-tests
couverture-et-tests: clean coverage test gcov

.PHONY: ctags
ctags:
	ctags $(SOURCES) $(DEPS)

.PHONY: clean
clean:
	rm -f $(EXEC)
	rm -rf $(DIROBJ)
	rm -f *.gc*
	rm -f *.log *.tmp
	rm -f tags core
