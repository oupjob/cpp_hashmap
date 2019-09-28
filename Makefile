SHELL		:= /bin/bash
PROJ_ROOT 	= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
MAKEFILE      	= Makefile
DEL_FILE      	= rm -f
SRC_DIR		= $(PROJ_ROOT)src
CITYHASH_SRC_DIR = $(SRC_DIR)/cityhash
TESTS_SRC_DIR 	= $(PROJ_ROOT)test
BUILD_DIR	= $(PROJ_ROOT)build
LIB_BUILD_DIR	= $(BUILD_DIR)/lib
TESTS_BUILD_DIR = $(PROJ_ROOT)build/test

LIB_SOURCES	= $(wildcard $(SRC_DIR)/*.cpp)
LIB_OBJS	= $(patsubst $(SRC_DIR)/%.cpp, $(LIB_BUILD_DIR)/%.cpp.o, $(LIB_SOURCES))
LIB		= $(LIB_BUILD_DIR)/libhashmap.so

TEST_SOURCES	= $(wildcard $(TESTS_SRC_DIR)/*.cpp)
TESTS_OBJS	= $(patsubst $(TESTS_SRC_DIR)/%.cpp, $(TESTS_BUILD_DIR)/.cpp.o%, $(TEST_SOURCES))
TESTS 		= $(patsubst $(TESTS_SRC_DIR)/%.cpp, $(TESTS_BUILD_DIR)/%, $(TEST_SOURCES))

CITYHASH_OBJ 	= $(LIB_BUILD_DIR)/city.cc.o

CXX		= g++
INCLUDE_DIRS    = $(SRC_DIR)
CXX_FLAGS	= -std=c++14 -I$(INCLUDE_DIRS) -Wextra -g -O3
CXX_LFLAGS	= -std=c++14 -Wall -g -O3 -L$(LIB_BUILD_DIR) -lhashmap
CXX_FLAGS_LIBHM = -std=c++14 -Wall -g -O3

lib: $(LIB)

tests: tests_clean $(CITYHASH_OBJ) lib $(TESTS) 
	
#$(CITYHASH_OBJ):
$(LIB_BUILD_DIR)/city.cc.o: $(CITYHASH_SRC_DIR)/city.cc
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

$(LIB_BUILD_DIR)/city.cc.pic.o: $(CITYHASH_SRC_DIR)/city.cc
	$(CXX) -c -fpic $(CXX_FLAGS) -o $@ $<
	
#$(LIB):
$(LIB_BUILD_DIR)/libhashmap.so: $(LIB_OBJS) $(LIB_BUILD_DIR)/city.cc.pic.o # $(LIB_BUILD_DIR)/%.cpp.o # $(LIB_BUILD_DIR)/city.cc.o
	$(CXX) -shared -fpic $(CXX_FLAGS_LIBHM) -o $@ $^
	
#$(LIB_OBJS):
$(LIB_BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -fpic $(CXX_FLAGS) -o $@ $<
	
#$(TESTS_OBJS):
$(TESTS_BUILD_DIR)/%.cpp.o: $(TESTS_SRC_DIR)/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

#$(TESTS):	
$(TESTS_BUILD_DIR)/%: $(TESTS_BUILD_DIR)/%.cpp.o $(LIB_BUILD_DIR)/city.cc.o
	$(CXX) $(CXX_LFLAGS) -o $@ $^
# 	@echo
# 	@echo 'Run a test: ' $@ 
# 	@LD_LIBRARY_PATH=$(LIB_BUILD_DIR) $@
# 	@if [[ $$? == 0 ]]; then \
# 		echo "Test: '`basename $@`' Completed Succesfully!"; \
# 	else \
# 		echo "Test: '`basename $@`' Failed!"; \
# 	fi
# 	@echo
	
tests_clean:
	$(DEL_FILE) $(TESTS_BUILD_DIR)/*
	
clean: tests_clean
	$(DEL_FILE) $(LIB_BUILD_DIR)/*

watch:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))
