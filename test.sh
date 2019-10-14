i=1
while cmp out out_correct 
do
    echo "$i" | python3 fib.py >out_correct
    echo "$i" | ./a.out >out
    echo "$i"
    i=$((i+1))
done