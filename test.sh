i=1
while cmp out out_correct 
do
    echo "$i" | python3 fib.py >out_correct
    echo "$i" | ./fib >out
    i=$((i+1))
done
echo "$i"