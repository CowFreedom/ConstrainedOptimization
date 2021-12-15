import constrained_optimization as co
import epidemics as epi

def f1(t0,tend,v,timepoints,data,err):
	print("Hey")

def f2(x,y):
	print("Drin playboy")
	return 

options=co.PSOOptions()
options.set_max_iterations(2)
options.set_n_groups(1)
options.set_n_particles(3)

enum=co.ConfigOutput.File

estimated_parameters=co.EVar64Manager()

seird_model=epi.SEIRDVARA_Model()

#print(epi.add_num(seird_model.get_evaluate_function()))

evaluator=co.EpidemicsEvaluation("./","subset_target.lua",seird_model.get_evaluate_function())
optimizer=co.ParticleSwarmOptimizerEpidemics(options,evaluator)
optimizer.run(estimated_parameters,list(["alpha"]),list([co.EFloat64(0),co.EFloat64(0.1)]))
#print(estimated_parameters)
