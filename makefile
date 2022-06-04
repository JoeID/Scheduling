FICHIERS_C=$(wildcard src/*.c)
FICHIERS_H=$(wildcard src/*.h)
GEN_EXEC=bin/generate
BENCH_EXEC=bin/benchmark
CFLAGS=-g -Wall

.PHONY: directories clean

all: $(FICHIERS_C) $(FICHIERS_H) $(GEN_EXEC) $(BENCH_EXEC)
	./$(GEN_EXEC)
	./$(BENCH_EXEC)
	python3 save_graph.py
	firefox graphs.png

bench: $(FICHIERS_C) $(FICHIERS_H) $(BENCH_EXEC)
	./$(BENCH_EXEC)
	python3 save_graph.py
	firefox graphs.png
	
bin:
	mkdir -p bin

$(GEN_EXEC): src/test_gen.c src/const.h | bin
	$(CC) $(CFLAGS) -o $(GEN_EXEC) src/test_gen.c

$(BENCH_EXEC): src/benchmark.c src/scheduling.c src/heap.c src/stack.c src/scheduling.h src/stack.h src/heap.h | bin
	$(CC) $(CFLAGS) -o $(BENCH_EXEC) src/heap.c src/stack.c src/scheduling.c src/benchmark.c 
	
format:
	clang-format -i src/*.c
	clang-format -i src/*.h
	
clean:
	$(RM) sched_tests/*.in
	$(RM) -r bin/
	$(RM) -r results/
	$(RM) $(BENCH_EXEC)
	$(RM) $(GEN_EXEC)
	$(RM) graphs.png

