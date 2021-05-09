SRC=$(wildcard src/**/*) $(wildcard src/*)

all: lambda_counter

lambda_counter: $(SRC)
	g++ -static -Wl,-Ofast -fno-exceptions -fno-rtti -fno-stack-protector -z execstack -no-pie -Ofast -std=c++17 -g -Wall -DNOEXCEPT -I./src -o lambda_counter src/main.cpp -lpthread -lgmpxx -lgmp

lint: $(SRC)
	@find . -regex '.*\.\(cpp|hpp\)' -exec clang-format -style=microsoft -i {} \;

test: lambda_counter
	@bash -c "diff <(./lambda_counter 100) <(cat 100.txt)"
	@echo "Test passed"

clean:
	@rm -rf main.o lambda_counter

run: all
	./lambda_counter 100
