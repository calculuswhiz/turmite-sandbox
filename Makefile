FLAGS = -Wall -std=c++11
LIBS = -lGL -lGLU -lglut
DEPS = Automaton.h Statenode.h

%.o: %.cpp $(DEPS)
	g++ -g -c $(FLAGS) -o $@ $< $(LIBS)

main: main.o Automaton.o Statenode.o
	g++ -g  $(FLAGS) -o main $^ $(LIBS)
	rm *.o

clean:
	rm *.o main
