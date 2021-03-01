import numpy as np
import matplotlib.pyplot as plt
import math


def SIR(t,u,gamma,c,lam):
	res=np.zeros(len(u))
	S=u[0]
	I=u[1]
	res[0]=-c*S*I
	res[1]=c*S*I-(gamma+lam)*I
#	print(gamma+lam)
	res[2]=gamma*I
	res[3]=lam*I
	return res
	
def calc_values(t,u,h,gamma,c,lam):
    k1=SIR(t,u,gamma,c,lam)
    k2=SIR(t+0.5*h,u+h*0.5*k1,gamma,c,lam)
    k3=SIR(t+0.5*h,u+h*0.5*k2,gamma,c,lam)
    k4=SIR(t+h,u+h*k3,gamma,c,lam)
    return u+(1/6.0)*h*(k1+2*k2+2*k3+k4)

def rk4(t0,u,tend,h,gamma,c,lam):
    res=np.zeros(shape=(int((tend-t0)/h+1),len(u)))
    ts=np.zeros(shape=(len(res)))
    t=t0+h
    ts[0]=t0
    i=0
    res[0]=u
    while t<=tend:
      #  print(t)
        ts[i+1]=t
        res[i+1]=calc_values(t,res[i],h,gamma,c,lam)
        t+=h
        i+=1
    
    if (ts[i]!=tend):
        t=tend
        res[i+1]=calc_values(t,res[i],h,gamma,c,lam)
        ts[i+1]=t
        print("HEYA")
     
    return (ts,res)
    
    
t0=0
tend=40
h=0.1
u=np.array([753056,2102,0,71])
#u=np.array([8*10**7,5*10**4,10**5,0])

#gamma=0.9*(np.log(0.1)/-14)
#c=(1.1*gamma)/(u[0]+u[1]+u[2])
#lam=0.1*(np.log(0.1)/-14) #death rate

data=np.loadtxt("expData/data_7_cities.csv",delimiter=",") #load real data
datat=np.arange(0,40)

tau=0.089
theta=0.0015 #scale factor for death rate
alpha=1.47e-7#scale factor for susceptibles
print("alpha:",alpha)
t,x=rk4(t0,u,tend,h,tau,alpha,theta)
plt.plot(datat,data[:,3],label ="Infected (real)",marker="o",linestyle="")
#plt.plot(t,x[:,0],label="Susceptible") #number of susceptible
plt.plot(t,x[:,1],label="Infected") #number of infected
#plt.plot(t,x[:,2],label="Recovered")
plt.plot(datat,data[:,10],label ="Deaths (real)",marker="o",linestyle="")
plt.plot(t,x[:,3],label="Deaths")
plt.xlabel("time in days")
plt.ylabel("Numer of people")
plt.title("Frankfurt data 40 days")
plt.legend()
print(x[:,1])
plt.show()
