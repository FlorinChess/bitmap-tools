COMPILER      := gcc
CCFLAGS       := -Wall -Wextra -Wuninitialized -pedantic -std=c17 -g3 -Og
PROGRAM       := bmp
BUILD_FOLDER  := build
.DEFAULT_GOAL := bin

TESTRUNNER_FLAGS := -c test.toml
TESTRUNNER_EXECUTABLE := ../testrunner

.PHONY: reset clean bin all run test help



clean:			## cleans up project folder
	@echo "[\033[36mINFO\033[0m] Cleaning up folder..."
	rm -f $(PROGRAM)
	rm -f $(PROGRAM).so
	find ./pictures/ -type f -name "*.txt" -print0 | xargs -0 rm

bin:			## compiles project to executable binary
	@echo "[\033[36mINFO\033[0m] Creating build folder..."
	mkdir -p $(BUILD_FOLDER)
	@echo "[\033[36mINFO\033[0m] Compiling binary..."
	$(COMPILER) $(CCFLAGS) -o ./$(BUILD_FOLDER)/$(PROGRAM) ./src/main.c
	chmod +x ./$(BUILD_FOLDER)/$(PROGRAM)

all: clean bin 	## all of the above
	
help:			## prints the help text
	@echo "Usage: make \033[36m<TARGET>\033[0m"
	@echo "Available targets:"
	@awk -F':.*?##' '/^[a-zA-Z_-]+:.*?##.*$$/{printf "  \033[36m%-10s\033[0m%s\n", $$1, $$2}' $(MAKEFILE_LIST)
