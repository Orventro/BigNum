i=1
while cmp out out_correct 
do
    echo "print(7**$i)" | python3 -q >out_correct
    echo "$i" | ./test_2 >out
    i=$((i+1))
done
echo "$i"