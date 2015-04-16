__author__ = 'Егор'
import matplotlib.pyplot as plt
from matplotlib.colors import BoundaryNorm
from matplotlib.ticker import MaxNLocator
import numpy as np

u = []
f = open("data.txt", 'r')
num = f.readline().split()
k = int(num[0])
m = int(num[1])
i = 0
for line in f:
    str_u = list(map(float, line.split()))
    if i <= k:
        u.append(str_u)
    elif i == k+2:
        x = str_u
    elif i == k+3:
        y = str_u
    i += 1


u = np.array(u)
x = np.array(x)
y = np.array(y)

levels = MaxNLocator(nbins=15).tick_values(u.min(), u.max())

cmap = plt.get_cmap('PiYG')
norm = BoundaryNorm(levels, ncolors=cmap.N, clip=True)

plt.contourf(x, y, u, levels=levels, cmap=cmap)
plt.colorbar()
plt.title('u(x,t)')

plt.show()

f.close()