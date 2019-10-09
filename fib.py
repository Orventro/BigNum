a = 1
b = 1
n = int(input())

for i in range(n-1):
    c = a + b
    a = b
    b = c

print(a)