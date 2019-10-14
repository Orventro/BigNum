s = 100000000000.0
x = s/2
for i in range(100):
    x -= (x*x-s)/(2*x)
    print(x)