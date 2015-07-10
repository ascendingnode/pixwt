from pixwt import pixwt,disk
import numpy as np
import matplotlib.pyplot as plt

x,y,r,n = 6.89,5.123,3.67,13

d = disk(x,y,r,n)
a = np.pi*r*r
s = np.sum(d)
print(a,s,s/a)

plt.imshow(d,origin='lower',interpolation='none')
plt.show()
