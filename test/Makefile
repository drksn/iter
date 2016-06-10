.PHONY: test clean remake

PREFIX=..

INCLUDE=-I./ \
		-I${PREFIX} \
		-I./googletest/include/

#CXX=g++-5

CXXFLAGS=-isystem -Wall --std=c++0x -DDEBUG -g

LIB=-L./googletest/ \
	-lgtest -lgtest_main

VPATH+=./

SRCS=

OBJS=$(patsubst %.cpp,%.o,$(SRCS))

all : build_gtest $(OBJS) test

build_gtest:
	cmake googletest
	make -C googletest

test: log_test link_test util_test

log_test: log_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

link_test: link_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

util_test: util_test.o $(OBJS)
	$(CXX) ${CXXFLAGS} $^ -o $@ $(LIB)

%.o:%.cpp
	$(CXX) -c ${INCLUDE} ${CXXFLAGS} $^

clean:
	rm -rf *.out *.o *.log *_test

remake: clean all
