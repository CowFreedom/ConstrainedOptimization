--[[
This is the evaluate.lua file. It is called by the computation subroutine if ConfigOutput in the Evaluation
class is set to File. All evaluations by the problem formulation happen here.
--]]


PrintBuildConfiguration()
ug_load_script("ug_util.lua")

-----------------------------------------------------------------
-- define Home-Directories
----------------------------------------------------------------
ug4_home        = ug_get_root_path().."/"
app_home        = ug4_home.."apps/parameteroptimization/"
common_scripts  = app_home.."scripts/"
geom_home       = app_home.."geometry/"

-----------------------------------------------------------------

--Load Parameters
local pars = "parameters.lua"

local parmfileloaded= false
if(pfile ~= "") then
	local file = assert(loadfile(pars))
	file()
	parmfileloaded = true
end
if parmfileloaded == false then
	print("Parameter file could not be loaded")
end
	

--Configure problem formulation with the new parameter values
v_alpha = parameters.alpha:get_value_as_double()
v_kappa = parameters.kappa:get_value_as_double()
v_theta = parameters.theta:get_value_as_double()
v_qq = parameters.qq:get_value_as_double()
v_pp = parameters.pp:get_value_as_double()

v_c=1
v_k=0

--v_alpha=0.118812645864508
--v_theta=0.114667564712574
--v_kappa= 0.0779868083048391
--v_qq=10.3675008925131
--v_pp= 6.10083348497

-- Execute main script
-----------------------------------------------------------------
print("Start of Epidemics-MAIN")
ug_load_script(common_scripts.."epidemics.lua")


