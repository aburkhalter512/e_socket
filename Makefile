C=gcc
C_FLAGS=-c -pedantic -mmacosx-version-min=10.6

L=clang
L_FLAGS=-lSystem -mmacosx-version-min=10.6

SRCS=e_socket.c
DEPS=$(patsubst %.c,%.h,$(SRCS))
OBJS=$(patsubst %.c,%.o,$(SRCS))

TEST_SRCS=e_socket.c coverage.c intercept.c test_main.c
TEST_DEPS=e_socket.h coverage.h intercept.h
TEST_OBJS=$(patsubst %.c,%.o,$(TEST_SRCS))
TEST_EXE=test_main.out

%.o: %.c
	$(C) $(C_FLAGS) $< -o $@

build: $(DEPS) $(OBJS)

buildDebug: C_FLAGS += -g
buildDebug: build

buildTest: C_FLAGS += -DTEST -g
buildTest: $(TEST_DEPS) $(TEST_OBJS)

linkTest: buildTest
	$(L) $(TEST_OBJS) $(L_FLAGS) -o $(TEST_EXE)

test: linkTest
	./$(TEST_EXE)

retest: clean test

clean:
	rm -f $(OBJS)
	rm -f $(TEST_OBJS)

cleanResults:
	rm *.log
	rm results-*
