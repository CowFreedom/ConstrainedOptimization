import numpy as np
from mpl_toolkits.mplot3d import Axes3D  
# Axes3D import has side effects, it enables using projection='3d' in add_subplot
import matplotlib.pyplot as plt
import random


#data=np.loadtxt("")
data=np.array([[0,0,1],[4,4,3]])
index1=0 #column of first parameter you want
index2=1 #column of second parameter you want

datax=data[:,index1]
datay=data[:,index2]
lossvals=data[:,len(data[0])-1]
chosenData=np.stack([datax,datay],axis=1)
print(chosenData)
def fun(x, y):
	minpos=0
	minval=(chosenData[0][0]-x)**2+(chosenData[0][1]-y)**2
	for i in range(1,len(chosenData)):
		print(str(minval)+"vs."+str((chosenData[i][0]-x)**2+(chosenData[i][1]-y)**2))
		if (chosenData[i][0]-x)**2+(chosenData[i][1]-y)**2:
			minpos=i
			minval=(chosenData[i][0]-x)**2+(chosenData[i][1]-y)**2
	return lossvals[minpos]   

def fun2(x,y):
	return x**2+y**2

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
x = y = np.arange(0, 4.0, 0.05)
X, Y = np.meshgrid(x, y)


xr=np.ravel(X)
yr=np.ravel(Y)
zs=np.zeros(shape=(len(xr)))
for i in range(len(yr)):
	zs[i]=fun(xr[i],yr[i])
#zs = np.array(fun2(np.ravel(X), np.ravel(Y)))
Z = zs.reshape(X.shape)
ax.plot_surface(X, Y, Z)

ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

plt.show()

