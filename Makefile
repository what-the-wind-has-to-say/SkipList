CC = g++
CXXFLAGS = -std=c++0x
main:main.o mySkipList.h
	$(CC) -o ./bin/main $^ --std=c++11 -pthread
	rm -f ./*.o
clean:
	rm -f ./*.o
	rm ./bin/main 
