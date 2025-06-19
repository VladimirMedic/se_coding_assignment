CC=gcc
CFLAGS=-Iinclude
SRC=$(wildcard src/*.c)
TEST=test/test_event_monitor.c
EXE=test_event_monitor.exe

all: $(EXE)

$(EXE): $(SRC) $(TEST)
	$(CC) $(CFLAGS) $^ -o $@

run: $(EXE)
	$(EXE)

clean:
	cmd /C "del /f /q $(EXE)"
