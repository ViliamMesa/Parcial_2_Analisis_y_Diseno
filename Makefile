CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = bd_sql
SRCS = main.cpp ArbolBPlus.cpp AnalizadorSQL.cpp
OBJS = $(SRCS:.cpp=.o)
TEST_SPLITS = test_splits

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_SPLITS): tests/test_splits.cpp ArbolBPlus.cpp ArbolBPlus.h
	$(CXX) $(CXXFLAGS) tests/test_splits.cpp ArbolBPlus.cpp -o $(TEST_SPLITS)

test_splits: $(TEST_SPLITS)
	./$(TEST_SPLITS)

clean:
	rm -f $(TARGET) $(OBJS) $(TEST_SPLITS) base_datos.txt test_splits_base.txt
