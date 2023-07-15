CC            := clang
CCFLAGS       := -Wall -Wextra -Wuninitialized -pedantic -std=c17 -g3 -Og
ASSIGNMENT    := a3
.DEFAULT_GOAL := help

TESTRUNNER_FLAGS := -c test.toml
TESTRUNNER_EXECUTABLE := ../testrunner

.PHONY: reset clean bin all run test help



clean:			## cleans up project folder
	@echo "[\033[36mINFO\033[0m] Cleaning up folder..."
	rm -f $(ASSIGNMENT)
	rm -f $(ASSIGNMENT).so
	rm -rf testreport.html
	rm -rf ./valgrind_logs
	rm -rf result.html

bin:			## compiles project to executable binary
	@echo "[\033[36mINFO\033[0m] Compiling binary..."
	chmod +x ./$(TESTRUNNER_EXECUTABLE)
	$(CC) $(CCFLAGS) -o $(ASSIGNMENT) $(ASSIGNMENT).c
	chmod +x $(ASSIGNMENT)

all: clean bin 	## all of the above

run: all		## runs the project with default config
	@echo "[\033[36mINFO\033[0m] Executing binary..."
	./$(ASSIGNMENT)

test: all		## runs public testcases on the project
	@echo "[\033[36mINFO\033[0m] Executing testrunner..."
	./$(TESTRUNNER_EXECUTABLE) $(TESTRUNNER_FLAGS)
testopen: all
	@echo "[\033[36mINFO\033[0m] Executing testrunner..."
	./$(TESTRUNNER_EXECUTABLE) $(TESTRUNNER_FLAGS) -b
	
testbychar: all             ## runs public testcases on the project compares output char by char
	@echo "[\033[36mINFO\033[0m] Executing testrunner..."
	./$(TESTRUNNER_EXECUTABLE) $(TESTRUNNER_FLAGS) -m c

testbyword: all             ## runs public testcases on the project compares output word by word
	@echo "[\033[36mINFO\033[0m] Executing testrunner..."
	./$(TESTRUNNER_EXECUTABLE) $(TESTRUNNER_FLAGS) -m w
	
testprint: all             ## runs public testcases on the project compares output line by line, prints to terminal
	@echo "[\033[36mINFO\033[0m] Executing testrunner..."
	./$(TESTRUNNER_EXECUTABLE) $(TESTRUNNER_FLAGS) -v
	
help:			## prints the help text
	@echo "Usage: make \033[36m<TARGET>\033[0m"
	@echo "Available targets:"
	@awk -F':.*?##' '/^[a-zA-Z_-]+:.*?##.*$$/{printf "  \033[36m%-10s\033[0m%s\n", $$1, $$2}' $(MAKEFILE_LIST)
