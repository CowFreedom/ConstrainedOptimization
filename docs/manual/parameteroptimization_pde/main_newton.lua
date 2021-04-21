ug_load_script("ug_util.lua")
local pfile = util.GetParam ("-p"			,	"") --gets argument after -p when calling script
local sfile = util.GetParam ("-s"			,	"")

if (pfile ~= "") then
	ug_load_script(pfile)
	print("LOADED "..pfile)
	pFileLoaded = true
end

-----------------------------------------------------------------
-- define Home-Directories
----------------------------------------------------------------
ug4_home        = ug_get_root_path().."/"
app_home        = ug4_home.."apps/parameteroptimization/"
common_scripts  = app_home.."scripts/"
geom_home       = app_home.."geometry/"

-----------------------------------------------------------------

--Defining the parameters
--[[
Parameters that need to be optimized: 
alpha =  ansteckungsrate 
Kappa =  ausbruchsrate 0<=kappa<=1
theta =   0 <=  Sterberate <= 1 -- https://coronavirus.jhu.edu/data/mortality
qq =  Inkubationszeit (in days) (5-6 from RKI (the median is 5-6 days))(95%der infizierten Symtome entwickelt,lag das 95.percentil der Inkubationszeit bei10-14)
pp=  duration of desease

]]

-- Set inital value, lower bound, upper bound for each parameter
v_alpha=EVar64(EFloat64(0.0924629297966215),EFloat64(0.00000001),EFloat64(10))
v_kappa=EVar64(EFloat64(0.1),EFloat64(0),EFloat64(1))
v_theta=EVar64(EFloat64(0.02),EFloat64(0),EFloat64(1))
v_qq=EVar64(EFloat64(14),EFloat64(5),EFloat64(16))
v_pp=EVar64(EFloat64(7),EFloat64(4),EFloat64(15))

--Add the parameters into the Parameter Manager 
--Note: Only the added parameters will be optimized
manager=EVar64Manager()
manager:add("alpha",v_alpha)
--[[manager:add("kappa",v_kappa)
manager:add("theta",v_theta)
manager:add("qq",v_qq)
manager:add("pp",v_pp)
]]
--Defining storage place for the the estimated paramteers
estimated_parameters=EVar64Manager()

--Running Newton Gauss
stepsize_alpha=1.0 --Hyperparameter of the adaptive stepsize calculation. See AdaDelta for details. The second hyperparamater (squared gradient decay rate) can be set by directly interfacing with the C++ code
print(stepsize_alpha)
RunNewtonGauss_BiogasEval("/home/devanshr/gcsc/ug4/apps/parameteroptimization/",manager,estimated_parameters,stepsize_alpha) 


--[[Generate a lua table with the parameters.
Note: The paramters are converted to double which means they lose
their accumulated error bounds.
--]]
tab={}
for k=0,estimated_parameters:len()-1 do
			
			name=estimated_parameters:get_name(k)
			value=estimated_parameters:get_param(k):get_value_as_double()
			tab[name]=value
end
print("Parameter values converted into lua table:")
print(tab)


print("Execution completed") 
