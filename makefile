FICHIERS_C=$(wildcard src/*.c)
FICHIERS_H=$(wildcard src/*.h)
BENCH_EXEC=bin/benchmark
CFLAGS=-O3 -Wall

.PHONY: directories clean

all: $(FICHIERS_C) $(FICHIERS_H) $(BENCH_EXEC)
	./$(BENCH_EXEC) -otf
	python3 save_graph.py
	firefox graphs.png 
	
bin:
	mkdir -p bin

tmp:
	mkdir -p tmp

count:
	wc -l src/*.c
	wc -l src/*.h

$(BENCH_EXEC): $(FICHIERS_C) $(FICHIERS_H) | bin
	$(CC) $(CFLAGS) -o $(BENCH_EXEC) $(FICHIERS_C) -pthread
	
format:
	clang-format -i src/*.c
	clang-format -i src/*.h
	
clean: tmp #removes everything except save_graph.py, README.md, src/, and makefile
	$(RM) sched_tests/*.in
	$(RM) -r bin/
	$(RM) -r results/
	$(RM) graphs.png
	cp makefile save_graph.py README.md tmp/
	-$(RM) *
	cp tmp/README.md tmp/save_graph.py tmp/makefile .
	$(RM) -r tmp/
	

