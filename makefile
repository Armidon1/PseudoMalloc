CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra
SRC_DIR=source
HEAD_DIR=headers
TEST_DIR=test
LDFLAGS=

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

TEST_SRCS=$(wildcard $(TEST_DIR)/*.c)
TEST_BINS=$(patsubst $(TEST_DIR)/%.c,%,$(TEST_SRCS))

.PHONY: all clean run

all: $(TEST_BINS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(HEAD_DIR) -c -o $@ $<

%: $(OBJS) $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -I$(HEAD_DIR) -o $@ $^ $(LDFLAGS)

run: all
	@for bin in $(TEST_BINS); do ./$$bin; done

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_BINS)
