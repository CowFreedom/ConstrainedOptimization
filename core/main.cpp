#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"


int main(){

	co::EVar<co::EFloat64> v_k_m_Propionic_Degradation(0.4888,0.000001,0.9);
	co::EVar<co::EFloat64> v_k_m_aceto_Methanogenesis(0.4268,0.00001,0.9);
	co::EVar<co::EFloat64> v_k_s_Propionic_Degradation(0.066,0.00001,0.95);
	co::EVar<co::EFloat64> v_k_s_aceto_Methanogenesis(0.141,0.1,0.9);
	co::EVar<co::EFloat64> v_general_decay_rate(8.3E-04,0.00001,0.9);
	co::EVar<co::EFloat64> v_Acetic(0.138,0.1,1);

	co::EVar<co::EFloat64> v_MO_acetoM(1.0,0.8,5);
	co::EVar<co::EFloat64> v_MO_Propionic(1.0,0.8,5);
	co::EVar<co::EFloat64> v_MO_ButyricValeric(1.0,0.8,5);	

	co::EVarManager<co::EFloat64> vars;
	vars.add("k_m_Propionic_Degradation",v_k_m_Propionic_Degradation);
	vars.add("k_m_aceto_Methanogenesis",v_k_m_aceto_Methanogenesis);
	vars.add("k_s_Propionic_Degradation",v_k_s_Propionic_Degradation);
	vars.add("k_s_aceto_Methanogenesis",v_k_s_aceto_Methanogenesis);
	vars.add("general_decay_rate",v_general_decay_rate);
	vars.add("Acetic",v_Acetic);

	vars.add("MO_acetoM",v_MO_acetoM);
	vars.add("MO_Propionic",v_MO_Propionic);
	vars.add("MO_ButyricValeric",v_MO_ButyricValeric);

	std::string path="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/estebis_downflow_diss_test1";
	co::run_parameter_estimator(path,"",vars);
}