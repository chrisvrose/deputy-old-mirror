include ../Makefile

# The list of tests is in ../runtests.pl

.PHONY: clean alltests

alltests:
	cd ..; ./testdeputy -r --nogroup slow --nogroup ALWAYS
#	@echo -e "\n\nAll tests were successful. \n\n"

clean:
	rm -f *.cil.c *.i *.exe *.o *~ *.stackdump *-tmp.c

ifndef NODEPUTY
OPTIONS := $(OPTIONS) --fail-stop
endif

runall/%: %.c
	COMMAND="$(DEPUTY) $(OPTIONS) \
                 -o __BASENAME__.exe __FILE__ && ./__BASENAME__.exe" \
	COMMENT="//" \
	  perl $(RUNALL) $*.c
