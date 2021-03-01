print("HIER")
--[[
This is the evaluate.lua file. It is called by the computation subroutine if ConfigOutput in the Evaluation
class is set to File. All evaluations by the problem formulation happen here.
--]]
--Load Parameters
local pars = "parameters.lua"
local file = assert(loadfile(pars))
file()

--Retrieve parameter values
v_alpha=parameters.alpha:get_value_as_double() --scale factor for susceptible derivative
v_tau=parameters.tau:get_value_as_double() --scale factor for recovered derivative
v_theta=parameters.theta:get_value_as_double() --scale factor death derivative


--Must be adjusted according to data in subset_target
--Initial values at time t0
u={}
u[1]=753056
u[2]=2102 -- Frankfurt infected
u[3]=0 -- Frankfurt deaths
u[4]=72 -- Frankfurt recovered

-----------------------------------------------------------------
print("Start of extended SIR model calculation")

local t0=0 --simulation start time
local tend=40 --simulation end time
local h=0.01 --step size for runge kutta order 4 method. Step size is fixed throughout calculation

function SIR(t,u,gamma,c,lam)
	res={}
	S=u[1]
	I=u[2]
	res[1]=-c*S*I
	res[2]=c*S*I-(gamma+lam)*I
	res[3]=gamma*I
	res[4]=lam*I
	return res
end

function calc_values(t,u,h,gamma,c,lam)
	k1=SIR(t,u,gamma,c,lam)
	v={}
	v[1]=u[1]+h*0.5*k1[1]
	v[2]=u[2]+h*0.5*k1[2]
	v[3]=u[3]+h*0.5*k1[3]
	v[4]=u[4]+h*0.5*k1[4]
	k2=SIR(t+0.5*h,v,gamma,c,lam)

	v[1]=u[1]+h*0.5*k2[1]
	v[2]=u[2]+h*0.5*k2[2]
	v[3]=u[3]+h*0.5*k2[3]
	v[4]=u[4]+h*0.5*k2[4]
	k3=SIR(t+0.5*h,v,gamma,c,lam)

	v[1]=u[1]+h*k3[1]
	v[2]=u[2]+h*k3[2]
	v[3]=u[3]+h*k3[3]
	v[4]=u[4]+h*k3[4]
	k4=SIR(t+h,v,gamma,c,lam)
	local result={}
	result[1]=u[1]+(1.0/6.0)*h*(k1[1]+2*k2[1]+2*k3[1]+k4[1])
	result[2]=u[2]+(1.0/6.0)*h*(k1[2]+2*k2[2]+2*k3[2]+k4[2])	
	result[3]=u[3]+(1.0/6.0)*h*(k1[3]+2*k2[3]+2*k3[3]+k4[3])
	result[4]=u[4]+(1.0/6.0)*h*(k1[4]+2*k2[4]+2*k3[4]+k4[4])
	return result
end
	
function rk4(t0,u,tend,h,gamma,c,lam)
	res={}
	res[1]=u[1]
	res[2]=u[2]
	res[3]=u[3]
	res[4]=u[4]
	ts={}
	t=t0+h
	i=1

	local result="#time\tSusceptible\tInfected\tRecovered\tDeaths\n"
	result=result..t.."\t"..res[1].."\t"..res[2].."\t"..res[3].."\t"..res[4]
	
	while t<=tend+1 do
		result=result.."\n"
		res=calc_values(t,res,h,gamma,c,lam)
		t=t+h
		i=i+1
		result=result..t.."\t"..res[1].."\t"..res[2].."\t"..res[3].."\t"..res[4]
	end
	

	file=io.open("sir_result.txt","w")
	file:write(result)
	file:close()
end	


--gamma=0.14
--c=(1.1*gamma)/(u[1]+u[2]+u[3])
--lam=0.016 --death rate
rk4(0,u,tend,h,v_tau,v_alpha,v_theta)

print("Calculation ended")
