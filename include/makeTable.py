import numpy as np

magFactor = 100

f = open('sineTable', 'w')

for i in xrange(360):
  f.write(str(int(magFactor * np.sin(i * np.pi/180))) + ",\n")

f.close()
