--Only parameters added to this container will be considered by the
--estimation procedure
manager=EVar64Manager()

--Defining parameters
--The first number is the initial parameter value and the following
--two numbers give lower and upper bounds that must not be exceeded by
--the optimization procedure
v1=EVar64(EFloat64(0.4268),EFloat64(1e-6),EFloat64(1.0))
manager:add("k_m_aceto_Methanogenesis",v1)

v2=EVar64(EFloat64(8.3E-04),EFloat64(1e-8),EFloat64(0.7))
manager:add("k_dec",v2)

v2=EVar64(EFloat64(0.141),EFloat64(1e-8),EFloat64(0.7))
manager:add("K_s_aceto_Methanogenesis",v2)

--When the estimation is finished, new parameter values can be accessed
--from Lua via tjis container
estimated_parameters=EVar64Manager()

--Running Newton Gauss
stepsize_alpha=1.0 --Hyperparameter of the adaptive stepsize calculation. See AdaDelta for details. The second hyperparamater (squared gradient decay rate) can be set by directly interfacing with the C++ code
RunNewtonGauss_BiogasEval("/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/testreactor_newton/",manager,estimated_parameters,stepsize_alpha)

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
