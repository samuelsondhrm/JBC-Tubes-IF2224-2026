CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic

# Paths
MS1_SRC  = src/lexer-ms1
MS1_OUT  = test/milestone-1/outputs
TARGET   = $(MS1_SRC)/arion-lexer

# Source files
SRCS = $(MS1_SRC)/main.cpp \
       $(MS1_SRC)/token/token.cpp \
       $(MS1_SRC)/lexer/symbolTable.cpp \
       $(MS1_SRC)/lexer/lexer.cpp \
       $(MS1_SRC)/dfa/dfa.cpp \
       $(MS1_SRC)/util/file.cpp

OBJS = $(SRCS:.cpp=.o)

# Test input files
TEST_VALID    			= test/milestone-1/test-valid.txt
TEST_INVALID  			= test/milestone-1/test-invalid.txt
TEST_EDGE     			= test/milestone-1/test-edgecases.txt
TEST_CASE_INSENSITIVE 	= test/milestone-1/test-caseinsensitive.txt
TEST_LITERAL_TYPES 		= test/milestone-1/test-literaltypes.txt
TEST_EQL				= test/milestone-1/test-eql.txt

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule: compile each .cpp to its own .o in the same directory
$(MS1_SRC)/%.o: $(MS1_SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Ensure output directory exists before running
$(MS1_OUT):
	mkdir -p $(MS1_OUT)

# Each target prints to terminal and writes to test/milestone-1/outputs/

run: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_VALID) $(MS1_OUT)/output-valid.txt

run-invalid: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_INVALID) $(MS1_OUT)/output-invalid.txt

run-edge: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_EDGE) $(MS1_OUT)/output-edgecases.txt

run-caseinsensitive: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_CASE_INSENSITIVE) $(MS1_OUT)/output-caseinsensitive.txt

run-literaltypes: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_LITERAL_TYPES)/output-literaltypes.txt

run-eql$(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_EQL) $(MS1_OUT)/output-eql.txt

run-all: $(TARGET) | $(MS1_OUT)
	./$(TARGET) $(TEST_VALID)   $(MS1_OUT)/output-valid.txt
	./$(TARGET) $(TEST_INVALID) $(MS1_OUT)/output-invalid.txt
	./$(TARGET) $(TEST_EDGE)    $(MS1_OUT)/output-edgecases.txt
	./$(TARGET) $(TEST_CASE_INSENSITIVE) $(MS1_OUT)/output-caseinsensitive.txt
	./$(TARGET) $(TEST_LITERAL_TYPES)/output-literaltypes.txt
	./$(TARGET) $(TEST_EQL) $(MS1_OUT)/output-eql.txt

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

clean-out:
	rm -f $(MS1_OUT)/output-valid.txt \
	      $(MS1_OUT)/output-invalid.txt \
	      $(MS1_OUT)/output-edgecases.txt

clean-all: clean clean-out

.PHONY: all run run-invalid run-edge run-all clean clean-out clean-all
