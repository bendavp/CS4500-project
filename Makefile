
HDRS := $(wildcard src/adapter/*.hh)
SRCS := $(wildcard src/adapter/*.cc) src/main.cc

.PHONY: clean docker run build valgrind

CXX_FLAGS := -g -Wall -std=c++17 -O3 -Iinclude -o eau2

eau2: $(SRCS) $(HDRS)
	g++ $(CXX_FLAGS) $(SRCS)

build:
	docker build -t eau2:0.1 .
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data; make eau2"

run: eau2
	./eau2 -f data/test.sor

docker: run

clean:
	rm -f eau2
	rm -rf eau2.dSYM

valgrind:
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data/; valgrind --leak-check=full ./eau2 -f data/test.sor"

# TESTS

.PHONY: test
test: build
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data ; make run_tests"

run_tests: test1 test2

.PHONY: test1
# Missing argument -f
test1: eau2
	- ./eau2 -f data/test.sor

.PHONY: test2
# Non-existant files - no or blank output expected
test2: eau2
	- ./eau2 -f data/test2.sor