import sys
import random
import math
from parse import picture

random.seed()
alpha = 1
beta = 1

o = picture()
o.parse(sys.stdin)
# Generate a basic solution
# Optimize the solution
for i in range(0, 10000):
    step()
# Print the best solution
o.display()



def setp():
    op = random.randint(0, 0)
    if op == 0:
        adds = []
        rms = []
    cost = 0
    if random.random() < alpha*math.exp(-beta * cost):
        pass #Executues modifs


