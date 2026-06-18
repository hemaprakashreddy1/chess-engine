g++ -Wall test.cpp ../src/Position.cpp -o test

if [ $? -eq 0 ]; then
    ./test < test.txt
fi