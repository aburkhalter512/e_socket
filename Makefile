C=clang
C_FLAGS=-c

L=clang
L_FLAGS=

SRCS=e_socket.c
DEPS=$(patsubst %.c,%.h,$(SRCS))
OBJS=$(patsubst %.c,%.o,$(SRCS))

TEST_SRCS=e_socket.c coverage.c intercept.c
TEST_DEPS=$(patsubst %.c,%.h,$(TEST_SRCS))
TEST_OBJS=$(patsubst %.c,%.o,$(TEST_SRCS))

%.o: %.c
	$(C) $(C_FLAGS) $< -o $@

build: $(DEPS) $(OBJS)

buildTest: C_FLAGS += -DTEST
buildTest: $(TEST_DEPS) $(TEST_OBJS)


clean:
	rm -f $(OBJS)
	rm -f $(TEST_OBJS)
