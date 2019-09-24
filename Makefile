SHELL		:= /bin/bash
PROJ_ROOT 	= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
MAKEFILE      	= Makefile
DEL_FILE      	= rm -f
SRC_DIR		= $(PROJ_ROOT)src
TESTS_SRC_DIR 	= $(PROJ_ROOT)test
BUILD_DIR	= $(PROJ_ROOT)build
TESTS_BUILD_DIR = $(PROJ_ROOT)build/test
TEST_SOURCES	= $(wildcard $(TESTS_SRC_DIR)/*.cpp)
TESTS 		= $(patsubst $(TESTS_SRC_DIR)/%.cpp, $(TESTS_BUILD_DIR)/%, $(TEST_SOURCES))

CXX		= g++
INCLUDE_DIRS    = $(SRC_DIR)
CXX_FLAGS	= -std=c++14 -I$(INCLUDE_DIRS) -Wall -g -O3
# CXX_LFLAGS	= 

tests: tests_clean $(TESTS)

$(TESTS_BUILD_DIR)/%: $(TESTS_SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -o $@ $<
	@echo 'run a test: ' $@ 
	@$@
	@if [[ $$? == 0 ]]; then \
		echo "Test: '`basename $@`' Completed Succesfully!"; \
	else \
		echo "Test: '`basename $@`' Failed!"; \
	fi
	
tests_clean:
	$(DEL_FILE) $(TESTS_BUILD_DIR)/*
	
watch:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))
