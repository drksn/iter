.PHONY: test clean remake

PREFIX=..

INCLUDE=-I./ \
		-I${PREFIX}/include \
		-I./googletest/include/

CXXFLAGS=-isystem -Wall --std=c++11 -pthread -DDEBUG -g

LIB=-L./googletest/ \
	-lgtest -lgtest_main

VPATH+=./

SRCS=

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

all : build_gtest $(OBJS) test

build_gtest:
	bash fetch_gtest.sh
	cd googletest && cmake . && make

test: log_test util_test file_keeper_test

log_test: log_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

util_test: util_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

file_keeper_test: file_keeper_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

%.o:%.cpp
	$(CXX) -c ${INCLUDE} ${CXXFLAGS} $^

clean:
	rm -rf *.out *.o *.log *_test *.test

remake: clean all
