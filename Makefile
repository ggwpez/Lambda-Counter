all: lambda_counter
	@echo "You can now run ./lambda_counter"

lambda_counter: main.o
	g++ -static -Wl,-O2 -o lambda_counter main.o -lpthread -lgmp

main.o:
	g++ -c -fno-exceptions -fno-rtti -std=c++17 -g -Wall -W -fPIC -DNOEXCEPT -I./src -o main.o src/main.cpp

clean:
	rm -rf main.o lambda_counter
