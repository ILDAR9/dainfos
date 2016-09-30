SOURCE = ./source
INCLUDE = ./include
LIB = ./lib
DOC = ./doc
RUN = ./run
MAIN = ./main

test:
	g++ -c $(SOURCE)/graph.cpp $(SOURCE)/rg.cpp $(SOURCE)/edge.cpp -I$(INCLUDE) -std=c++11
	rm -f $(LIB)/libgraph.a
	ar r $(LIB)/libgraph.a graph.o rg.o edge.o
	rm *.o
	g++ $(MAIN)/main.cpp -L$(LIB) -I$(INCLUDE) -lgraph -o $(RUN)/test -std=c++11

