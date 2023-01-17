g++ -c test.cpp -o test.o -fopenmp
g++ -Werror test.o -o test -fopenmp -lpthread
./test