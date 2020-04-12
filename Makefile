all: main

%: %.bf main
	./main < $< > $*.s
	as $*.s -o $*.o
	ld $*.o -o $@
