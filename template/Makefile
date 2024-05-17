depGraph: main.c main.h graph.o
	gcc -o depGraph main.c main.h graph.o

graph.o: graph.c graph.h
	gcc -c graph.c

run: depGraph
	./depGraph

.PHONY: clean

clean: 
	rm graph.o depGraph
