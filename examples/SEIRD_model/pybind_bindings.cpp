#include <pybind11/pybind11.h>
#include "pybind11/numpy.h"
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <iostream>
#include <functional>
#include <string>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include "../../../Epidemics/models/seird_variable_alpha.h"
#include <thread>


	void testf(double t_start, double t_end, const co::EVarManager<co::EFloat64>& v, std::vector<co::EFloat64>& timepoints, std::vector<co::EFloat64>& data,co::ErrorCode& err){
		std::cout<<89<<"\n"<<"\n";
std::cout<<"drin\n";
	std::vector<double> alpha={4.74716e-07,(double)v.get_param(0).val };
	std::vector<double> alpha_limits={50};
	double kappa=0.903178;
	double theta=4.14932000304998e-07;
	double qq=3.01401;
	double pp=6.0785;
	std::vector<double> u0={764000,2*28,28,0,0}; // Inital Values: Susceptibles, Exposed, Infected, Recoverd, Deceased	
	
		//Create model class and run the simulation
		std::vector<double> cumulated_infected;
		std::vector<double> cumulated_exposed;	
		ug::epi::SEIRD_VARA<std::vector<double>> seird_model(alpha, alpha_limits,kappa,theta,qq,pp);
		seird_model.change_step_size_time(0.125);
		auto result=seird_model.run_linear_implicit(t_start,u0,t_end,&cumulated_exposed,&cumulated_infected);
		
		//The output of the SEIRD Model must be filtered to the relevant classes with respect to the real life data set
		//Filter accordingly		
		std::vector<double> timepoints_temp=std::get<0>(result);
		std::vector<co::EFloat64> data_filtered;
		timepoints=std::vector<co::EFloat64>();	
		for (int i=0;i<timepoints_temp.size();i++){
			timepoints.push_back(co::EFloat64(timepoints_temp[i]));
			data_filtered.push_back(cumulated_infected[i]);    //Filter cumulative Infected			
		}

		data=data_filtered;

		err= co::ErrorCode::NoError;

	}

class SEIRDVARA_Model{
	public:	
	
	void evaluate_model(double t_start, double t_end, const co::EVarManager<co::EFloat64>& v, std::vector<co::EFloat64>& timepoints, std::vector<co::EFloat64>& data,co::ErrorCode& err){
		std::cout<<89<<"\n"<<"\n";

		std::cout<<"drin\n";
		//Set up parameters
		alpha[1]=(double)v.get_param(0).val;
		alpha_limits[0]={50};

		//Create model class and run the simulation
		std::vector<double> cumulated_infected;
		std::vector<double> cumulated_exposed;	
		ug::epi::SEIRD_VARA<std::vector<double>> seird_model(alpha, alpha_limits,kappa,theta,qq,pp);
		seird_model.change_step_size_time(0.125);
		auto result=seird_model.run_linear_implicit(t_start,u0,t_end,&cumulated_exposed,&cumulated_infected);
		
		//The output of the SEIRD Model must be filtered to the relevant classes with respect to the real life data set
		//Filter accordingly		
		std::vector<double> timepoints_temp=std::get<0>(result);
		std::vector<co::EFloat64> data_filtered;
		timepoints=std::vector<co::EFloat64>();	
		for (int i=0;i<timepoints_temp.size();i++){
			timepoints.push_back(co::EFloat64(timepoints_temp[i]));
			data_filtered.push_back(cumulated_infected[i]);    //Filter cumulative Infected			
		}

		data=data_filtered;

		err= co::ErrorCode::NoError;

	}
	
	SEIRDVARA_Model(){	
		//std::cout<<"Pointer: "<<this<<"\n";		
	}
	
	std::vector<double> alpha={4.74716e-07,5.74716e-07 };
	std::vector<double> alpha_limits={50};
	double kappa=0.903178;
	double theta=4.14932000304998e-07;
	double qq=3.01401;
	double pp=6.0785;

	//Set up initial values for the S E I R D classes
	std::vector<double> u0={764000,2*28,28,0,0}; // Inital Values: Susceptibles, Exposed, Infected, Recoverd, Deceased	

	
	std::function<void(double, double, const co::EVarManager<co::EFloat64>&, std::vector<co::EFloat64>&, std::vector<co::EFloat64>&,co::ErrorCode&)> get_evaluate_function(){
		
		using std::placeholders::_1;
		using std::placeholders::_2;	
		using std::placeholders::_3;	
		using std::placeholders::_4;	
		using std::placeholders::_5;
		using std::placeholders::_6;	
		
		//return std::bind(&SEIRDVARA_Model::evaluate_model,this,_1,_2,_3,_4,_5,_6);
	//return std::bind(testf,0,_1,_2,_3,_4,_5,_6);	
		return testf;

	//	auto f2=[this](double t_start, double t_end, const co::EVarManager<co::EFloat64>& v, std::vector<co::EFloat64>& timepoints, std::vector<co::EFloat64>& data,co::ErrorCode& err){ evaluate_model(t_start,t_end,v,timepoints,data,err);};

	//	return testf;
	}
	~SEIRDVARA_Model(){		
		//std::cout<<"Object destroyed\n";	
	}	
	
};

//void add_num(std::function<void(double, double, const co::EVarManager<co::EFloat64>&, std::vector<co::EFloat64>&, std::vector<co::EFloat64>&,co::ErrorCode&)> eval_function){
void add_num(std::function<void(double, double, const co::EVarManager<co::EFloat64>&, std::vector<co::EFloat64>&, std::vector<co::EFloat64>&,co::ErrorCode&)> eval_function){	
	double t0=0;
	double tend=61;
	co::EVar64Manager v;
	std::vector<co::EFloat64> timepoints;
	std::vector<co::EFloat64> data;	
	co::ErrorCode err;
	
	auto test=SEIRDVARA_Model();
	
		//std::thread t1=std::thread(&TestClass::adder,&test,t0,tend,std::ref(v),std::ref(timepoints),std::ref(data),std::ref(err));
	std::thread t1=std::thread(test.get_evaluate_function(),t0,tend,std::ref(v),std::ref(timepoints),std::ref(data),std::ref(err));
	//std::thread t1=std::thread(eval_function,t0,tend);
	t1.join();
}



namespace py = pybind11;

PYBIND11_MODULE(epidemics, m) {
              
    py::class_<SEIRDVARA_Model> (m, "SEIRDVARA_Model")
        .def(py::init<>())
        .def("evaluate_model",&SEIRDVARA_Model::evaluate_model)   
        .def("get_evaluate_function",&SEIRDVARA_Model::get_evaluate_function,py::call_guard<py::gil_scoped_release>())             
        .def_readwrite("kappa", &SEIRDVARA_Model::kappa)
        .def_readwrite("theta", &SEIRDVARA_Model::theta)
		.def_readwrite("qq", &SEIRDVARA_Model::qq)
		.def_readwrite("pp", &SEIRDVARA_Model::pp)
        ;
         
        
        m.def("add_num",add_num,py::call_guard<py::gil_scoped_release>());
	
}

