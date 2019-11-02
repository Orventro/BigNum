n = int(input())
s = int(input())
for i in range(n):
    o, c = input().split()
    c = int(c)
    if   o == '+':
        s += c
    elif o == '-':
        s -= c
    elif o == '/':
        s //= c
    elif o == '*':
        s *= c
    elif o == '%':
        s %= c
    print(s)