import math

a = 7
b = 23
p = 3571
m = 1280

def h(x, p = p, a = a, b = b):
    return (a * x + b) % p

def H(x, m = m, p = p, a = a, b = b):
    return h(x, p, a, b) % m

def mod_back(x, m):
    return (m * (x // m) + (x % m))

def h_back(x, p = p, a = a, b = b):
    return mod_back(a * x + b, p); 

def H_back(x, m = m, p = p, a = a, b = b):
    return mod_back(h(x), m)

def x_back(x, p = p, a = a, b = b):
    return (p * ((a*x + b) // p) + h(x) - b) / a

def C(x, i):
    return (h(x) + m * i) % m

def print_c(x, n):
    for i in range(n):
        y = (h(x) + m * i) 
        print("h(x=%d) + (m=%d) * %d) mod (p=%d) = %d (y = %d)" % (x, m, i, p, C(x, i), y))
 
