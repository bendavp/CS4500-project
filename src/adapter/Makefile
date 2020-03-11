
HDRS := $(wildcard include/*.hh)
SRCS := $(wildcard src/*.cc)

.PHONY: clean docker run build

CXX_FLAGS := -g -Wall -std=c++17 -O3 -Iinclude -o sorer

sorer: $(SRCS) $(HDRS)
	g++ $(CXX_FLAGS) $(SRCS)

build:
	docker build -t sorer:0.1 .
	docker run -ti -v "`pwd`":/test sorer:0.1 bash -c "cd /test ; make sorer"

run: sorer
	./sorer -f file.txt

docker: run

clean:
	rm -f sorer *.o

# TESTS

.PHONY: test
test: build
	docker run -ti -v "`pwd`":/test sorer:0.1 bash -c "cd /test ; make run_tests"

run_tests: test1 test2 test3 test4 test5 test6 test7 test8 test9

.PHONY: test1
# Missing argument -f
test1: sorer
	- ./sorer

.PHONY: test2
# Non-existant files - no or blank output expected
test2: sorer
	- ./sorer -f tests/nonexistant.sor # no output
	- ./sorer -f tests/nonexistant.sor -print_col_type 0 # blank line output

.PHONY: test3
# Are spaces trimmed?
test3: sorer
	- ./sorer -f tests/test3.sor -print_col_type 0 # FLOAT
	- ./sorer -f tests/test3.sor -print_col_type 1 # STRING
	- ./sorer -f tests/test3.sor -print_col_type 2 # STRING
	- ./sorer -f tests/test3.sor -print_col_idx 0 0 # 2.0
	- ./sorer -f tests/test3.sor -print_col_idx 1 0 # "hello"
	- ./sorer -f tests/test3.sor -print_col_idx 2 0 # "asdf"

.PHONY: test4
# Simple well-formed file - are all types detected?
# Does -print_col_idx work?
# Does -is_missing_idx work?
test4: sorer
	# types
	- ./sorer -f tests/test4.sor -print_col_type 0 # BOOL
	- ./sorer -f tests/test4.sor -print_col_type 1 # STRING
	- ./sorer -f tests/test4.sor -print_col_type 2 # INT
	- ./sorer -f tests/test4.sor -print_col_type 3 # BOOL
	- ./sorer -f tests/test4.sor -print_col_type 4 # FLOAT
	- ./sorer -f tests/test4.sor -print_col_type 5 # FLOAT
	- ./sorer -f tests/test4.sor -print_col_type 6 # INT
	- ./sorer -f tests/test4.sor -print_col_type 7 # INT
	- ./sorer -f tests/test4.sor -print_col_type 8 # STRING
	- ./sorer -f tests/test4.sor -print_col_type 9 # UNKNOWN
	# print
	- ./sorer -f tests/test4.sor -print_col_idx 0 0 # 1
	- ./sorer -f tests/test4.sor -print_col_idx 1 0 # "hello"
	- ./sorer -f tests/test4.sor -print_col_idx 2 0 # 1
	- ./sorer -f tests/test4.sor -print_col_idx 3 0 # 0
	- ./sorer -f tests/test4.sor -print_col_idx 4 0 # 0.5
	- ./sorer -f tests/test4.sor -print_col_idx 5 0 # 0.0
	- ./sorer -f tests/test4.sor -print_col_idx 6 0 # -5
	- ./sorer -f tests/test4.sor -print_col_idx 7 0 # 5423
	- ./sorer -f tests/test4.sor -print_col_idx 8 0 # "goodbye friend"
	- ./sorer -f tests/test4.sor -print_col_idx 9 0 # print nothing
	# is_missing
	- ./sorer -f tests/test4.sor -is_missing_idx 0 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 1 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 2 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 3 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 4 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 5 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 6 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 7 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 8 0 # 1
	- ./sorer -f tests/test4.sor -is_missing_idx 9 0 # 0

.PHONY: test5
# Two empty columns picked discarded?
test5: sorer
	- ./sorer -f tests/test5.sor -print_col_type 0 # error
	- ./sorer -f tests/test5.sor -print_col_type 1 # error
	- ./sorer -f tests/test5.sor -print_col_idx 0 0 # error
	- ./sorer -f tests/test5.sor -print_col_idx 1 0 # error
	- ./sorer -f tests/test5.sor -is_missing_idx 0 0 # error
	- ./sorer -f tests/test5.sor -is_missing_idx 1 0 # error

.PHONY: test6
# Schema detection
# 1. Promote type from BOOL -> INT -> FLOAT -> STRING
# 2. Anything past 500 lines shouldn't be detected
test6: sorer
	- ./sorer -f tests/test6.sor -print_col_type 0 # BOOL
	- ./sorer -f tests/test6.sor -print_col_type 1 # INT
	- ./sorer -f tests/test6.sor -print_col_type 2 # FLOAT
	- ./sorer -f tests/test6.sor -print_col_type 3 # UNKNOWN
	- ./sorer -f tests/test6.sor -print_col_type 4 # STRING
	- ./sorer -f tests/test6.sor -print_col_type 5 # STRING

.PHONY: test7
# Big file
test7: sorer
	- ./sorer -f tests/test7.sor -print_col_type 0 # INT
	- ./sorer -f tests/test7.sor -print_col_type 1 # STRING
	- ./sorer -f tests/test7.sor -print_col_type 2 # FLOAT
	- ./sorer -f tests/test7.sor -print_col_type 3 # BOOLEAN

.PHONY: test8
test8: sorer
	- ./sorer -f tests/test8.sor -from 1 -print_col_type 0 # STRING
	- ./sorer -f tests/test8.sor -from 1 -print_col_type 1 # STRING
	- ./sorer -f tests/test8.sor -from 1 -print_col_idx 0 0 # "0"
	- ./sorer -f tests/test8.sor -from 1 -print_col_idx 1 0 # "1"

.PHONY: test9
test9: sorer
	- ./sorer -f tests/test9.sor -len 10 -print_col_type 0 # STRING
	- ./sorer -f tests/test9.sor -len 10 -print_col_type 1 # STRING
	- ./sorer -f tests/test8.sor -len 10 -print_col_idx 0 0 # "string"
	- ./sorer -f tests/test8.sor -len 10 -print_col_idx 1 0 # "string2"
