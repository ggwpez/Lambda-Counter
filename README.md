### Prerequisites

You will need GNU GMP, its the big numbers libary used.  
Download it from here: https://gmplib.org/#DOWNLOAD  
You will also need qmake, otherwise just compile and link it manually, its just one cpp (-lgmpxx -lgmp).  

### Compile on GNU/Linux
```
git clone https://github.com/ggwpez/Lambda-Counter && cd Lambda-Counter && qmake src/lambda_counter.pro && make
```

### Usage
```
./lambda_counter [start=0] end
```
calculates { f(start), ... ,f(end) } where *start* is defaulted to 0.  
The format is the standard OEIS format:

start f(start)  
.  
.  
.  
end f(end)

### General

Code sometimes maybe looks obfuscated, but thats only due to optimisation.  
Feel free to fork.
