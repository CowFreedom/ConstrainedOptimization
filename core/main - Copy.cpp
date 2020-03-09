#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"


int main(){
	co::EVar<co::EFloat64> v_acetic(0.0033332640274,0.0000000001,200);
	//co::EVar<co::EFloat64> v_mo_acetom(1.37389497692,0.0000000001,200);
	co::EVar<co::EFloat64> v_k_dec(8.3E-04,0.0000000001,200);
	co::EVar<co::EFloat64> v_k_m_aceto_methanogenesis(0.42681,0.0000000001,200);
	co::EVarManager<co::EFloat64> vars;

	vars.add("Acetic",v_acetic);
	//vars.add("MO_acetoM",v_mo_acetom);
	vars.add("k_dec",v_k_dec);
	vars.add("aceto_Methanogenesis",v_k_m_aceto_methanogenesis);
	std::string path="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/estebis_downflow_test1";
	co::run_parameter_estimator(path,"",vars);
	//co::EFloat<double>(5);
	//std::cout<<var.min;
}