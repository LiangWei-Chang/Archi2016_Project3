CC = g++ -std=c++11 -Wall -Wextra

CMP: CMP.o Binary2Assembly.o Execution.o GlobalVar.o Processor.o
	${CC} -o CMP CMP.o Binary2Assembly.o Execution.o GlobalVar.o Processor.o
Processor.o: Processor.cpp GlobalVar.h Binary2Assembly.h
	${CC} -c -g Processor.cpp
GlobalVar.o: GlobalVar.cpp GlobalVar.h
	${CC} -c -g GlobalVar.cpp
Binary2Assembly.o: Binary2Assembly.cpp Binary2Assembly.h GlobalVar.h Execution.h
	${CC} -c -g Binary2Assembly.cpp
Execution.o: Execution.cpp GlobalVar.h Execution.h
	${CC} -c -g Execution.cpp	
CMP.o: CMP.cpp GlobalVar.h 
	${CC} -c -g CMP.cpp	
clean:
	rm -f Processor.o GlobalVar.o Binary2Assembly.o Execution.o CMP.o

