## Abstract
The three types of terms of the [Lambda-Calculus](https://en.wikipedia.org/wiki/Lambda_calculus) can be categorised by various constraints.  
One such constraints is the the length of their standart representation.  
This program counts the number of closed Lambda terms with a given length.  
This implementation is currently by far the best.  
ref OEIS [A260661](https://oeis.org/A260661).   

## Try it out

### Docker
```sh
docker run --rm -it ggwpez/lambda_counter 100
```

### Compiling on GNU/Linux

You will need GNU GMP, its the big numbers libary used.  
Download it from [here](https://gmplib.org/#DOWNLOAD) or install the package *libgmp-dev*.
```sh
git clone https://github.com/ggwpez/Lambda-Counter && cd Lambda-Counter && make
```
## Usage
It takes two arguments; the *start* and the *end*. It caluculates the values in range [*start*, *end*] .  
The *start* can be omitted in which case it is defaulted to *0*.
Example:
```sh
./lambda_counter 10 100
```
Can be cancelled with `Ctrl+C`.  
[100.txt](100.txt) contains the output for [0, 100].

## License
Licensed under GPLv3, see [LICENSE](LICENSE) file.  
Copyright 2016-2021 Oliver Tale-Yazdi <oliver@tasty.limo>.
