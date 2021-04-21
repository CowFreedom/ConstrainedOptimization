--[[
This is the evaluate.lua file. It is called by the computation subroutine if ConfigOutput in the Evaluation
class is set to File. All evaluations by the problem formulation happen here.
--]]
--Load Parameters
local pars = "parameters.lua"

local file = assert(loadfile(pars))
file()


--Retrieve parameter values
v_theta1=parameters.v_theta1:get_value_as_double()
v_theta2=parameters.v_theta2:get_value_as_double()
v_theta3=parameters.v_theta3:get_value_as_double()


-----------------------------------------------------------------
print("Start of logistic1 calculation")

local time_end=20
local delta=0.5

function logistic(theta1, theta2,theta3,x)
	return theta1/(1+math.exp(-theta2*x))+theta3
end

file=io.open("produced_deGFP.txt","w")
result="#time val val"

for i=0,time_end/delta do
	result=result.."\n"
	result=result..i*delta.."\t"..logistic(v_theta1,v_theta2,v_theta3,i*delta)
end


--local output= io.open(produced_deGFP.txt, "w")
file:write(result)
file:close()

print("logistic1 calculation ended")
