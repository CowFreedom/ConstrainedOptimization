--[[
This is the evaluate.lua file. It is called by the computation subroutine if ConfigOutput in the Evaluation
class is set to File. All evaluations by the problem formulation happen here.
--]]

PrintBuildConfiguration()
ug_load_script("ug_util.lua") 

-----------------------------------------------------------------
-- define Home-Directories
-----------------------------------------------------------------
ug4_home			=	ug_get_root_path().."/"
app_home			=	ug4_home.."apps/biogas_app/"
common_scripts			= 	app_home.."scripts/"
geom_home			= 	app_home.."geometry/"

-----------------------------------------------------------------

--Load biogas problem
local pfile = app_home.."Test.lua"

local pFileLoaded = false
if (pfile ~= "") then
	ug_load_script(pfile)
	print("LOADED "..pfile)
	pFileLoaded = true
end

--Load Parameters
local pars = "parameters.lua"

local parameterFileLoaded = false
if (pfile ~= "") then
	local file = assert(loadfile(pars))
	file()
	print("LOADED "..pars)
	parameterFileLoaded = true
end
if parameterFileLoaded == false then
	print("Parameters could not be loaded")
end

--Configure problem formulation with the new parameter values
--problem.expert.reactionParams.k_m={}

if problem.expert==nil then
	problem.expert={}
end

if problem.expert.reactionParams==nil then
	problem.expert.reactionParams={}
end

if problem.expert.reactionParams.k_m==nil then
	print("Fall A1")
	problem.expert.reactionParams.k_m={}
	problem.expert.reactionParams.k_m["aceto_Methanogenesis"]=parameters.k_m_aceto_Methanogenesis:get_value_as_double()
else
	problem.expert.reactionParams.k_m["aceto_Methanogenesis"]=parameters.k_m_aceto_Methanogenesis:get_value_as_double()
end

if problem.expert.reactionParams.K_s==nil then
	print("Fall B1")
	problem.expert.reactionParams.K_s={}
	problem.expert.reactionParams.K_s["aceto_Methanogenesis"]=parameters.K_s_aceto_Methanogenesis:get_value_as_double()
else
	problem.expert.reactionParams.K_s["aceto_Methanogenesis"]=parameters.K_s_aceto_Methanogenesis:get_value_as_double()
end


if problem.expert.reactionParams.k_dec==nil then
	print("Fall C1")
	problem.expert.reactionParams.k_dec={}
	problem.expert.reactionParams.k_dec["MO_acetoM"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_hydroM"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_ButyricValeric"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_Propionic"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoMS"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoAA"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoLCFA"]=parameters.k_dec:get_value_as_double()
else
	problem.expert.reactionParams.k_dec["MO_acetoM"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_hydroM"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_ButyricValeric"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_Propionic"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoMS"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoAA"]=parameters.k_dec:get_value_as_double()
	problem.expert.reactionParams.k_dec["MO_AcidoLCFA"]=parameters.k_dec:get_value_as_double()
end

-- Execute main script
-----------------------------------------------------------------
print("Start of Biogas-MAIN")
ug_load_script(common_scripts.."Main.lua")


