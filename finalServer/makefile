tester: commInterface.o ourServer2.0.o spreadsheet.o
	g++ -std=gnu++11 -lpthread -o a.out commInterface.o ourServer2.0.o spreadsheet.o
commInterface.o: commInterface.cpp commInterface.h 
	g++ -c -std=gnu++11 commInterface.cpp
ourServer2.0.o: ourServer2.0.cpp
	g++ -c -std=gnu++11 ourServer2.0.cpp 
spreadsheet.o:	spreadsheet.cpp
	g++ -c -std=gnu++11 spreadsheet.cpp
