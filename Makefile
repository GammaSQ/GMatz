
SOURCES = test.cpp

hypertest: $(SOURCES)
	g++ -g -o hypertest $< -pedantic -Wall
