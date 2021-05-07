--Only parameters added to this container will be considered by the
--estimation procedure
manager=VarDescriptor64()

--Defining parameters
--The first number is the initial parameter value and the following
--two numbers give lower and upper bounds that must not be exceeded by
--the optimization procedure

manager:add("k_m_aceto_Methanogenesis",EFloat64(1e-6),EFloat64(0.5))
manager:add("K_s_aceto_Methanogenesis",EFloat64(0.1),EFloat64(0.5))
manager:add("k_dec",EFloat64(8e-04),EFloat64(0.1))

--When the estimation is finished, new parameter values can be accessed
--from Lua via this container
estimated_parameters=EVar64Manager()

--Running the Particle Swarm Optimization Algorithm
n_particles=12
n_groups=2
max_iterations=20 --maximum iterations of the PSO algorithm if no convergence is reached beforehand
RunPSO_BiogasEval("/home/devanshr/Programs/ug4/plugins/ConstrainedOptimization/examples/testreactor_pso",manager,estimated_parameters,n_particles,n_groups,max_iterations)

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
