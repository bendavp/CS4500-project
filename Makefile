SRCS := src/main.cpp

.PHONY: clean docker run build valgrind

CXX_FLAGS := -g -Wall -std=c++17 -O3 -Iinclude -pthread

eau2: $(SRCS)
	g++ $(CXX_FLAGS) $(SRCS) -o eau2

dataframe: 
	g++ $(CXX_FLAGS) tests/dataframe-tests.cpp -o dataframe

kvstore:
	g++ $(CXX_FLAGS) tests/kv-tests.cpp -o kvstore

build:
	docker build -t eau2:0.1 .
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data; make eau2"

run: eau2
	./eau2 -f data/100k.txt -n 2

docker: run

clean:
	rm -f eau2
	rm -rf eau2.dSYM
	rm dataframe
	rm -rf dataframe.dSYM
	rm kvstore
	rm -rf kvstore.dSYM

valgrind:
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data/; valgrind --leak-check=full ./eau2 -f data/test.sor"

# TESTS

.PHONY: test
test: build
	docker run -ti -v "`pwd`":/data eau2:0.1 bash -c "cd /data ; make run_tests"

run_tests: test1 test2 test3 test4

.PHONY: test1
# test.sor
test1: eau2
	- ./eau2 -f data/test.sor

.PHONY: test2
# test2.sor
test2: eau2
	- ./eau2 -f data/test2.sor

.PHONY: test3
# dataframe tests
test3: dataframe
	- ./dataframe

.PHONY: test4
# kvstore tests
test4: kvstore
	- ./kvstore
