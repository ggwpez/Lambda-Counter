##Compile on GNU/Linux:

You will need GNU GMP, its the big numbers libary used.
You can get it here: https://gmplib.org/#DOWNLOAD.

cd src/ && qmake && make all

##Use:

./lambda_counter [start=0] end

will output { f(start), ... ,f(end) } in the OEIS format, like

n f(n)

##General:

Code sometimes maybe looks obfuscated, but thats only due to optimisation.
Feel free to make suggestions on how to transform it into a recursive algorithm,
or other general improvements.