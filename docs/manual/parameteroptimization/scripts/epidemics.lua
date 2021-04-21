-- Copyright (c) 2020:  G-CSC, Goethe University Frankfurt
-- Authors: Arne Naegel, Gabriel Wittum
-- 
-- This file is part of UG4.
-- 
-- UG4 is free software: you can redistribute it and/or modify it under the
-- terms of the GNU Lesser General Public License version 3 (as published by the
-- Free Software Foundation) with the following additional attribution
-- requirements (according to LGPL/GPL v3 §7):
-- 
-- (1) The following notice must be displayed in the Appropriate Legal Notices
-- of covered and combined works: "Based on UG4 (www.ug4.org/license)".
-- 
-- (2) The following notice must be displayed at a prominent place in the
-- terminal output of covered works: "Based on UG4 (www.ug4.org/license)".
-- 
-- (3) The following bibliography is recommended for citation and must be
-- preserved in all covered files:
-- "Reiter, S., Vogel, A., Heppner, I., Rupp, M., and Wittum, G. A massively
--   parallel geometric multigrid solver on hierarchically distributed grids.
--   Computing and visualization in science 16, 4 (2013), 151-164"
-- "Vogel, A., Reiter, S., Rupp, M., Nägel, A., and Wittum, G. UG4 -- a novel
--   flexible software system for simulating pde based models on high performance
--   computers. Computing and visualization in science 16, 4 (2013), 165-179"
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU Lesser General Public License for more details.


local myPath = os.getenv("UG4_ROOT").."/apps/parameteroptimization"
package.path = package.path..";".. myPath.."/config/?.lua;".. myPath.."/?.lua;"..myPath.."/geometry/?.lua"

-----------------------------------------------------------------
-- define Home-Directories
----------------------------------------------------------------
ug4_home        = ug_get_root_path().."/"
app_home        = ug4_home.."apps/parameteroptimization/"
common_scripts  = app_home.."scripts/"
geom_home       = app_home.."geometry/"

-----------------------------------------------------------------

-- Load utility scripts (e.g. from from ugcore/scripts)
ug_load_script("ug_util.lua")
ug_load_script("util/refinement_util.lua")


-- Parse parameters and print help
util.CheckAndPrintHelp("Example: Spread of Epidemics (2D)");
local ARGS = {
   numRefs    = util.GetParamNumber("--num-refs", 0, "number of refinements"),
   problemID    = util.GetParam("--problem-id", "corona_DE_7Cities", "problem id"),
   debugLevel    = util.GetParamNumber("--debug-level", 0, "problem id"),
   limexNumStages =  util.GetParamNumber("--limex-num-stages", 2, "Number of stages"),
   withBlocks =  util.HasParamOption("--with-blocks", true),
   withPeriodicIdentification = util.HasParamOption("--with-periodic-identification", true, "adaptiveTimeStepping"),
}

-- initialize ug with the world dimension dim=2 and the algebra type
local blockSize = 1
if (ARGS.withBlocks) then blockSize=5 end
InitUG(2, AlgebraType("CPU", blockSize));  

local problem = require(ARGS.problemID)


-- Load a domain without initial refinements.
local dom = problem:create_domain()


-- Refine the domain (redistribution is handled internally for parallel runs)
print("refining...")
util.refinement.CreateRegularHierarchy(dom, ARGS.numRefs, true)
-- PrintIdentification( dom:grid() )
-----------------------------------------
-- A) Model parameters -- ############DEFINED in evaluate.lua #############
-----------------------------------------

--[[local 	v_alpha = 4.0       -- Ansteckunsrate    -- 4.0 (per day and fraction)
local v_kappa = 0.1       -- 0 <=  Ausbruchsrate <= 1
local v_theta = 0.02      -- 0 <=  Sterberate <= 1
local v_qq = 14.0         -- Inkubationszeit (in days)
local v_pp = 7.0          -- Dauer der Krankheit (in days)]]

-----------------------------------------
-- B) Ansatz spaces
-----------------------------------------
local approxSpace = ApproximationSpace(dom)
approxSpace:add_fct("g", "Lagrange", 1)   -- lineare Ansatzfunktionen
approxSpace:add_fct("a", "Lagrange", 1)
approxSpace:add_fct("k", "Lagrange", 1)
approxSpace:add_fct("r", "Lagrange", 1)
approxSpace:add_fct("v", "Lagrange", 1)
approxSpace:init_levels()
approxSpace:init_top_surface()

print("approximation space:")
approxSpace:print_statistic()


-----------------------------------------
-- C) Finite volume element discretization
-----------------------------------------

-- Define reaction terms (GLOBALLY!)
-- r_gg := - alpha * gg *aa -- Ansteckungsrate == alpha
function ReactionG(g,a) return v_alpha*g*a end --problem.reactionParams.alphaa*g*a
function ReactionG_dg(g,a) return v_alpha*a end
function ReactionG_da(g,a) return v_alpha*g end

-- r_aa := + (alpha*gg - 1/qq) * aa -- inkubationzeit == qq
function ReactionA(g,a) return (-1.0) * (v_alpha*g - 1.0/v_qq)*a end
--function ReactionA_dg(g,a) return (-1.0) * (problem.reactionParams.alpha*a)  end
--function ReactionA_da(g,a) return (-1.0) * (problem.reactionParams.alpha*g - 1.0/problem.reactionParams.qq)  end
-- Vereinfachte Jacobi-Matrix => Stabileres Verfahren!
function ReactionA_dg(g,a) return (-1.0) * (0.0*v_alpha*a)  end
function ReactionA_da(g,a) return (-1.0) * (0.0*v_alpha*g - 1.0/v_qq)  end

-- r_kk := \kappa/qq  * aa - kk / pp  
function ReactionK(a,k) return (-1.0) * ((v_kappa/v_qq) * a - k/v_pp) end
function ReactionK_da(a,k) return (-1.0) *(v_kappa/v_qq)  end
function ReactionK_dk(a,k) return (-1.0) * (-1.0/v_pp) end

-- r_rr := (1-\kappa)/qq  * aa + (1-theta)/pp * kk   
function ReactionR(a,k) return (-1.0) * ((1-v_kappa)/v_qq*a + (1.0-v_theta)/v_pp*k)  end
function ReactionR_da(a,k) return (-1.0) * ((1-v_kappa)/v_qq )  end
function ReactionR_dk(a,k) return (-1.0) * ((1.0-v_theta)/v_pp)  end

-- r_vv := /pp * kk   

-- Create elem discs per subsets (TODO: => C++)
local function create_elem_discs(subdom, diffusion, rho)

-- Create
local type = "fv1"
local elemDisc = {}
elemDisc["g"] = ConvectionDiffusion("g", subdom, type)  -- cell conc.
elemDisc["a"] = ConvectionDiffusion("a", subdom, type)  -- potential
elemDisc["k"] = ConvectionDiffusion("k", subdom, type)  -- 
elemDisc["r"] = ConvectionDiffusion("r", subdom, type)  -- 
elemDisc["v"] = ConvectionDiffusion("v", subdom, type)  -- 

-- Mass scale
elemDisc["g"]:set_mass_scale(rho)  -- default
elemDisc["a"]:set_mass_scale(rho)  -- default
elemDisc["k"]:set_mass_scale(rho)  -- default
elemDisc["r"]:set_mass_scale(rho)  -- default
elemDisc["v"]:set_mass_scale(rho)  -- default

-- Diffusion
elemDisc["g"]:set_diffusion(diffusion)  -- gesund
elemDisc["a"]:set_diffusion(diffusion)  -- angesteckt
elemDisc["r"]:set_diffusion(diffusion)  -- recovered
elemDisc["k"]:set_diffusion(0.0)        -- krank
elemDisc["v"]:set_diffusion(0.0)        -- verstorben


-- Reactions
local nonlinearGrowth = {}
nonlinearGrowth["g"] = LuaUserFunctionNumber("ReactionG", 2)
nonlinearGrowth["g"]:set_input(0, elemDisc["g"]:value())
nonlinearGrowth["g"]:set_input(1, elemDisc["a"]:value())
nonlinearGrowth["g"]:set_deriv(0, "ReactionG_dg")
nonlinearGrowth["g"]:set_deriv(1, "ReactionG_da")

nonlinearGrowth["a"] = LuaUserFunctionNumber("ReactionA", 2)
nonlinearGrowth["a"]:set_input(0, elemDisc["g"]:value())
nonlinearGrowth["a"]:set_input(1, elemDisc["a"]:value())
nonlinearGrowth["a"]:set_deriv(0, "ReactionA_dg")
nonlinearGrowth["a"]:set_deriv(1, "ReactionA_da")

nonlinearGrowth["k"] = LuaUserFunctionNumber("ReactionK", 2)
nonlinearGrowth["k"]:set_input(0, elemDisc["a"]:value())
nonlinearGrowth["k"]:set_input(1, elemDisc["k"]:value())
nonlinearGrowth["k"]:set_deriv(0, "ReactionK_da")
nonlinearGrowth["k"]:set_deriv(1, "ReactionK_dk")

nonlinearGrowth["r"] = LuaUserFunctionNumber("ReactionR", 2)
nonlinearGrowth["r"]:set_input(0, elemDisc["a"]:value())
nonlinearGrowth["r"]:set_input(1, elemDisc["k"]:value())
nonlinearGrowth["r"]:set_deriv(0, "ReactionK_da")
nonlinearGrowth["r"]:set_deriv(1, "ReactionK_dk")


elemDisc["g"]:set_reaction(rho*nonlinearGrowth["g"])            
elemDisc["a"]:set_reaction(rho*nonlinearGrowth["a"])  
elemDisc["k"]:set_reaction(rho*nonlinearGrowth["k"])  
elemDisc["r"]:set_reaction(rho*nonlinearGrowth["r"])  
-- -- elemDisc["r"]:set_source(rho* ((1.0-problem.reactionParams.theta) + (1-problem.reactionParams.kappa)/problem.reactionParams.qq) * elemDisc["k"]:value())  

-- r_vv := problem.reactionParams.theta/pp * kk   
elemDisc["v"]:set_source(rho*(v_theta/v_pp)*elemDisc["k"]:value())   

return elemDisc
end


local function add_elem_discs(problem, domainDisc)

local elemDisc = {}
for k,region in ipairs(problem.regions) do
  
  local regRho = region.density
  local regDiffusion = ScaleAddLinkerMatrix()
  regDiffusion:add(region.diffusion, regRho)
  elemDisc = create_elem_discs(region.subset, regDiffusion, regRho)
  
  domainDisc:add(elemDisc["g"])
  domainDisc:add(elemDisc["a"])
  domainDisc:add(elemDisc["k"])
  domainDisc:add(elemDisc["r"])
  domainDisc:add(elemDisc["v"])
end


end

-----------------------------------------
-- F) Domain discretization
-----------------------------------------
local domainDisc = DomainDiscretization(approxSpace)
add_elem_discs(problem, domainDisc)
-- domainDisc:add(dirichletBND)

-----------------------------------------
-- G) Solver setup
--    (using 'util/solver_util.lua')
-----------------------------------------
local solverDesc = {
	
	-- Newton's method for non-linear problem
	type = "newton",

 -- [[ 
  lineSearch =
  {
    type = "standard",
      maxSteps    = 6,
      lambdaStart   = 1,
      lambdaReduce  = 0.5,
      acceptBest    = false,
      checkAll    = false
   
  },--]]
  
  -- LU decomposition
  linSolver = SuperLU() -- for Newton debugging
  
  -- BiCGStab 
	--[[ 
	linSolver = {
	 type = "bicgstab",
	
  precond = {
		  type		= "gmg",
		  approxSpace	= approxSpace,
		  smoother	= "sgs",
		  baseSolver	= "lu"
	 },
	
  },--]]
	
}

-----------------------------------------
-- H) Solve transient problem
-----------------------------------------

local nlsolver = util.solver.CreateSolver(solverDesc)
print (nlsolver)

local dbgWriter = nil 
if (ARGS.debugLevel>0) then
  dbgWriter = GridFunctionDebugWriter(approxSpace)
  nlsolver:set_debug(dbgWriter)
end

print("\nsolving...")
local A = AssembledLinearOperator(domainDisc)
local u = GridFunction(approxSpace)
local b = GridFunction(approxSpace)

local vtk = VTKOutput()
-- Set initial values.
problem:set_initial_values(u)
vtk:print("InitialData", u)
SaveVectorCSV(u,"initail_distribution.csv")

vtk:select_element(problem.myCompositeDensity, "density");
--density = vtk:select_element(problem.myCompositeDensity, "pop_den");

-- Output Density Table for each subset: 

file = io.open("densities_ini.csv", 'w')
result = ""
result = result .. "index, Name, Density \n"
for index, val in pairs(problem.regions) do
  local si = dom:subset_handler():get_subset_index(val.subset)
  local name = val.subset
  result = result.. si .. ","..name.."," .. val.density .. "\n"
  --print(result)
end
print(result)
file:write(result)
file:close()

local startTime = 0
local endTime = 40.0
local dt = (endTime-startTime)/500


if (ARGS.limexNumStages<2) then

  -- Standard fixed size time-stepping

  --[[util.SolveNonlinearTimeProblem(u, domainDisc, nlsolver, vtk,
  "Covid", "ImplEuler", 1, startTime, endTime, dt, dt*0.001);
   ]]


-- create time disc [Implicit Euler]
local timeDisc = util.CreateTimeDisc(domainDisc, "ImplEuler", 1)

local time = startTime
local step = 0
local nlsteps = 0;
local maxStepSize = dt
local minStepSize = dt*0.001
local reductionFactor = 0.5

vtk: print("Solution", u, step, time)

--writing intial step 0 in csv 
output_file = "simdata_step_"..step ..".csv"
output_density = "density_step_"..step..".csv"
SaveVectorCSV(u, output_file) -- creates csv 

-- in each csv file add time, for each row.
local file = io.open(output_file, 'r')
local fileContent = {}
for line in file:lines() do 
   table.insert(fileContent, line)
end
io.close(file)
for i=1 , #fileContent do 
  if ( i == 1 ) then 
  else 
    fileContent[i] = time .. ", ".. fileContent[i]
  end
end
local file = io.open(output_file, 'w')
for index, line in ipairs(fileContent) do
  file:write(line.."\n")
end
io.close(file)

file = io.open(output_density, 'w')
result= ""
result = result .. "Time,Density,Index \n"
for index, val in pairs(problem.regions) do
   local si = dom:subset_handler():get_subset_index(val.subset)
   local name = val.subset
   result = result..time..",".. val.density .. ","..si.. "\n"

end
file:write(result)
file:close()

-- store grid function in vector of  old solutions
local solTimeSeries = SolutionTimeSeries()
solTimeSeries:push(u:clone(), time)

-- update newtonSolver	
nlsolver:init(AssembledOperator(timeDisc, u:grid_level()))

-- store old solution (used for reinit in multistep)
local uOld
if minStepSize <= maxStepSize * reductionFactor or newtonLineSearchFallbacks ~= nil then
  uOld = u:clone()
end

local relPrecisionBound = 1e-12

while (endTime == nil or ((time < endTime) and ((endTime-time)/maxStepSize > relPrecisionBound))) do
  step = step+1
  print("++++++ TIMESTEP " .. step .. " BEGIN (current time: " .. time .. ") ++++++");
  
  local solver_call = 0;
  
  -- initial t-step size
  local currdt = maxStepSize
  if endTime ~= nil then
    -- adjust in case of over-estimation
    if time+currdt > endTime then currdt = endTime-time end
    -- adjust if size of remaining t-domain (relative to `maxStepSize`) lies below `relPrecisionBound`
    if ((endTime-(time+currdt))/maxStepSize < relPrecisionBound) then currdt = endTime-time end
  end
  
  -- try time step
  local bSuccess = false;	
  while bSuccess == false do
    TerminateAbortedRun()

    print("++++++ Time step size: "..currdt);

    local newtonSuccess = false
    local newtonTry = 1
    --nlsolver:set_line_search(defaultLineSearch)

    -- try to solve the non-linear problem with different line-search strategies
    while newtonSuccess == false do
      -- get old solution if the restart with a smaller time step is possible
      if uOld ~= nil then
        VecAssign(u, uOld)
      end

      				-- setup time Disc for old solutions and timestep size
			timeDisc:prepare_step(solTimeSeries, currdt)
			
					-- prepare newton solver
      if nlsolver:prepare(u) == false then
        						print ("\n++++++ Newton solver failed."); exit();
      end 
      
      --print(u)
			-- apply newton solver
			newtonSuccess = nlsolver:apply(u)
						
			-- start over again if failed
      if newtonSuccess == false then break end

    	-- push oldest solutions with new values to front, oldest sol pointer is poped from end	
			local oldestSol = solTimeSeries:oldest()
			VecAssign(oldestSol, u)
			solTimeSeries:push_discard_oldest(oldestSol, timeDisc:future_time())          

    end

    if newtonSuccess == false then
      currdt = currdt * reductionFactor;
      write("\n++++++ Newton method failed. ");
      write("Trying decreased stepsize " .. currdt .. ".\n");					
      if(bSuccess == false and currdt < minStepSize) then
        write("++++++ Time Step size "..currdt.." below minimal step ")
        write("size "..minStepSize..". Cannot solve problem. Aborting.");
        test.require(false, "Time Solver failed.")
      end
      bSuccess = false
    else
      -- update new time
      time = timeDisc:future_time()
      nlsteps = nlsteps + nlsolver:num_newton_steps() 	 
      bSuccess = true
    end
  
  end -- while bSuccess == false

  -- save this solution if the restarted with a smaller time step is possible
  if uOld ~= nil then
    VecAssign (uOld, u)
  end			

  output_file_new = "simdata_step_"..step ..".csv"
  output_density_new = "density_step_"..step..".csv"

  SaveVectorCSV(u, output_file_new) -- creates csv 

  local file = io.open(output_file_new, 'r')
  local fileContent = {}
  for line in file:lines() do 
    table.insert(fileContent, line)
  end
  io.close(file)

  for i=1 , #fileContent do 

    if ( i == 1 ) then 
    else 
      fileContent[i] = time .. ", ".. fileContent[i]
    end

  end
  
  local file = io.open(output_file_new, 'w')
  for index, line in ipairs(fileContent) do
    file:write(line.."\n")
  end
  io.close(file)

  file = io.open(output_density_new, 'w')
  result = ""
  result = result .. "Time,Density,Index \n"
  for index, val in pairs(problem.regions) do
    local si = dom:subset_handler():get_subset_index(val.subset)
    local name = val.subset
    result = result..time..",".. val.density .. ","..si.. "\n"

  end

  file:write(result)
  file:close()
  vtk: print("Solution", u, step, time)
  --##############################################################################################################
  print("++++++ TIMESTEP "..step.." END   (current time: " .. time .. ") ++++++");
end

vtk:write_time_pvd("Sol_PVD", u)
ug_load_script(common_scripts.."convert_values_7cities.lua")
ug_load_script(common_scripts.."multiplier.lua")

-- Limex time stepping
else

  local step = 0
  local time = 0
  --writing intial step 0 in csv 
  output_file = "simdata_step_"..step ..".csv"
  output_density = "density_step_"..step..".csv"
  SaveVectorCSV(u, output_file) -- creates csv 
  
  --  in each csv file add time, for each row.
  local file = io.open(output_file, 'r')
  local fileContent = {}
  for line in file:lines() do 
     table.insert(fileContent, line)
  end
  io.close(file)
  for i=1 , #fileContent do 
    if ( i == 1 ) then 
    else 
      fileContent[i] = time .. ", ".. fileContent[i]
    end
  end
  local file = io.open(output_file, 'w')
  for index, line in ipairs(fileContent) do
    file:write(line.."\n")
  end
  io.close(file)

  file = io.open(output_density, 'w')
  result= ""
  result = result .. "Time,Density,Index \n"
  for index, val in pairs(problem.regions) do
     local si = dom:subset_handler():get_subset_index(val.subset)
     local name = val.subset
     result = result..time..",".. val.density .. ","..si.. "\n"
  end
  file:write(result)
  file:close()

    -- LIMEX time stepping
    local nstages = ARGS.limexNumStages
    local limex = LimexTimeIntegrator(nstages) 
 
    local limexConvCheck=ConvCheck(1, 5e-8, 1e-10, true)
    limexConvCheck:set_supress_unsuccessful(true)
    
    local limexNLSolver = {}
    for i=1,nstages do 
      limexNLSolver[i] = util.solver.CreateSolver(solverDesc)
      --limexNLSolver[i]:set_linear_solver(SuperLU())
      limexNLSolver[i]:disable_line_search()
      limexNLSolver[i]:set_convergence_check(limexConvCheck)
      limex:add_stage(i, limexNLSolver[i], domainDisc)
    end
    
    -- DEBUG output
    if (ARGS.debugLevel>0) then 
      limexNLSolver[1]:set_debug(dbgWriter)
    end
    
    -- Konfiguration des Fehlerschätzers
    local concErrorEst = CompositeGridFunctionEstimator()
  
    if (false) then
      -- L2 Fehler
      concErrorEst:add(L2ComponentSpace("a", 2))
      concErrorEst:add(L2ComponentSpace("g", 2))
      concErrorEst:add(L2ComponentSpace("k", 2))
      concErrorEst:add(L2ComponentSpace("r", 2))  
      concErrorEst:add(L2ComponentSpace("v", 2)) 
    end
  
    if (true) then
      -- H1 Halbnorm
      concErrorEst:add(H1SemiComponentSpace("a", 2))
      concErrorEst:add(H1SemiComponentSpace("g", 2))
      concErrorEst:add(H1SemiComponentSpace("k", 2))
      concErrorEst:add(H1SemiComponentSpace("r", 2))  
      concErrorEst:add(H1SemiComponentSpace("v", 2)) 
    end
   limex:add_error_estimator(concErrorEst)
   
   -- Tolerance
   limex:set_tolerance(5e-3)
    
   limex:set_time_step(dt)
   -- limex:set_dt_min(dtmin)
   -- limex:set_dt_max(dtmax)
   limex:set_increase_factor(2.0)
   limex:set_stepsize_safety_factor(0.8)
    

   --post processing after each step
     function postProcess(u, step, time, currdt)

      output_file_new = "simdata_step_"..step ..".csv"
      output_density_new = "density_step_"..step..".csv"

      SaveVectorCSV(u, output_file_new) -- creates csv 

      local file = io.open(output_file_new, 'r')
      local fileContent = {}
      for line in file:lines() do 
        table.insert(fileContent, line)
      end
      io.close(file)

      for i=1 , #fileContent do 
        if ( i == 1 ) then 
        else 
          fileContent[i] = time .. ", ".. fileContent[i]
        end
      end
  
      local file = io.open(output_file_new, 'w')
      for index, line in ipairs(fileContent) do
        file:write(line.."\n")
      end
      io.close(file)

      file = io.open(output_density_new, 'w')
      result = ""
      result = result .. "Time,Density,Index \n"
      for index, val in pairs(problem.regions) do
        local si = dom:subset_handler():get_subset_index(val.subset)
        local name = val.subset
        result = result..time..",".. val.density .. ","..si.. "\n"
      end

      file:write(result)
      file:close()
    end

   local luaObserver = LuaCallbackObserver()
   function luaPostProcess(step, time, currdt)
    postProcess(luaObserver:get_current_solution(), step, time, currdt)
    return 0;
   end

   luaObserver:set_callback("luaPostProcess")
    -- Ausgabe
    local limexVTKObserver = VTKOutputObserver("LimexSol.vtk", vtk)
    limex:attach_observer(limexVTKObserver)
    limex:attach_observer(luaObserver)
    
    -- Start des Verfahrens
    limex:apply(u,endTime, u,startTime)
    ug_load_script(common_scripts.."convert_values_7cities.lua")
    ug_load_script(common_scripts.."multiplier.lua")
end
print("done")

