ashell: main.cpp ashell.o CommandHistory.o Tokenizer.o pwd.o ff.o ls.o
	g++ -std=c++11 -o ashell main.cpp ashell.o CommandHistory.o Tokenizer.o pwd.o ff.o ls.o

ashell.o: ashell.cpp ashell.h
	g++ -std=c++11 -c ashell.cpp

CommandHistory.o: CommandHistory.cpp CommandHistory.h
	g++ -std=c++11 -c CommandHistory.cpp

Tokenizer.o: Tokenizer.cpp Tokenizer.h
	g++ -std=c++11 -c Tokenizer.cpp

pwd.o: pwd.cpp pwd.h
	g++ -std=c++11 -c pwd.cpp

ff.o: ff.cpp ff.h
	g++ -std=c++11 -c ff.cpp

ls.o: ls.cpp ls.h
	g++ -std=c++11 -c ls.cpp

clean:
	rm *.o ashell
