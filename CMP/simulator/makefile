CC = g++ -std=c++11 -Wall -Wextra

CMP: CMP.o Binary2Assembly.o Execution.o Processor.o
	${CC} -o CMP CMP.o Binary2Assembly.o Execution.o  Processor.o
Processor.o: Processor.cpp Instruction.h Binary2Assembly.h
	${CC} -c -g Processor.cpp
Binary2Assembly.o: Binary2Assembly.cpp Binary2Assembly.h Instruction.h Execution.h
	${CC} -c -g Binary2Assembly.cpp
Execution.o: Execution.cpp Instruction.h Execution.h
	${CC} -c -g Execution.cpp	
CMP.o: CMP.cpp Instruction.h 
	${CC} -c -g CMP.cpp	
clean:
	rm -f Processor.o  Binary2Assembly.o Execution.o CMP.o

