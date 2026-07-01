: square dup * ;
5 square .
cr

: fact dup 1 > if dup 1 - fact * else drop 1 then ;
6 fact .
cr

: fizzbuzz 16 1 do
    i 15 mod 0 = if ." FizzBuzz "
    else i 3 mod 0 = if ." Fizz "
    else i 5 mod 0 = if ." Buzz "
    else i .
    then then then
  loop cr ;
fizzbuzz

variable x
10 x !
5 x +!
x @ .
cr

words
bye
