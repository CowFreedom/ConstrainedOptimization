import numpy as np
import matplotlib.pyplot as plt

data=np.loadtxt("expData/Verstorbene.txt")

plt.plot(data[0:,],data[1:,])
plt.show