FICHIERS_C=$(wildcard src/*.c)
FICHIERS_H=$(wildcard src/*.h)
GEN_EXEC=bin/generate
BENCH_EXEC=bin/benchmark
CFLAGS=-O3 -Wall

.PHONY: directories clean

all: $(FICHIERS_C) $(FICHIERS_H) $(GEN_EXEC) $(BENCH_EXEC)
	./$(GEN_EXEC)
	./$(BENCH_EXEC)
	python3 save_graph.py
	firefox graphs.png

test:
	$(CC) -pg -Wall -g src/heap.c src/stack.c src/taskload_tree.c src/scheduling.c src/scheduling_mael.c src/benchmark.c 
	
bin:
	mkdir -p bin

tmp:
	mkdir -p tmp

count:
	wc -l src/*.c
	wc -l src/*.h

$(GEN_EXEC): src/test_gen.c src/const.h | bin
	$(CC) $(CFLAGS) -o $(GEN_EXEC) src/test_gen.c

$(BENCH_EXEC): src/benchmark.c src/scheduling.c src/heap.c src/f_offsets.c src/stack.c src/taskload_tree.c src/scheduling_mael.c src/scheduling_mael.h src/taskload_tree.h src/scheduling.h src/stack.h src/heap.h src/f_offsets.c | bin
	$(CC) $(CFLAGS) -o $(BENCH_EXEC) src/heap.c src/f_offsets.c src/stack.c src/taskload_tree.c src/scheduling.c src/scheduling_mael.c src/benchmark.c 
	
format:
	clang-format -i src/*.c
	clang-format -i src/*.h
	
clean: tmp #removes everything except save_graph.py, README.md, src/, makefile
	$(RM) sched_tests/*.in
	$(RM) -r bin/
	$(RM) -r results/
	$(RM) graphs.png
	cp makefile save_graph.py README.md tmp/
	-$(RM) *
	cp tmp/README.md tmp/save_graph.py tmp/makefile .
	$(RM) -r tmp/
	

