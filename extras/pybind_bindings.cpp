#include <pybind11/pybind11.h>
#include "pybind11/numpy.h"
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include<iostream>
#include "../core/parameter_estimation.h"
#include "../core/parameters.h"
#include <string>




namespace py = pybind11;

PYBIND11_MODULE(constrained_optimization, m) {
              
    py::class_<co::EFloat64>(m, "EFloat64")
        .def(py::init<double>())
        .def(py::self + py::self)
        .def(py::self - py::self)  
        .def(py::self * py::self)  
        .def(py::self / py::self)                  
        .def("__repr__",
        [](const co::EFloat64 &v) {
            return "EFloat64{ val:" + std::to_string(v.get_v())+",low:" + std::to_string(v.lower_bound())+"high:" + std::to_string(v.upper_bound())  + "}";
        }); 
       
     py::class_<co::EVar64>(m, "EVar64")
        .def(py::init<const co::EFloat64,const co::EFloat64,const co::EFloat64>())                
        .def("__repr__",
        [](const co::EVar64 &v) {
            return v.to_lua_string();
        });        
        
    py::class_<co::EVarManager<co::EFloat64>>(m, "EVar64Manager")
        .def(py::init<>())
        .def(py::init<std::vector<co::EVar<co::EFloat64>>, std::vector<std::string>>())
        .def("add",&co::EVarManager<co::EFloat64>::add)
        .def("get_params",&co::EVarManager<co::EFloat64>::get_params)
        .def("contains",&co::EVarManager<co::EFloat64>::contains)
        ;         	
        
    py::class_<co::PSOOptions>(m, "PSOOptions")
        .def(py::init<>())
        .def("set_max_iterations",&co::PSOOptions::set_max_iterations)
        .def("set_n_particles",&co::PSOOptions::set_n_particles)
        .def("set_n_groups",&co::PSOOptions::set_n_groups)
        .def("__repr__",
        [](const co::PSOOptions &v) {
            return "PSOOptions{ max_iterations:" + std::to_string(v.get_max_iterations())+",n_particles:" + std::to_string(v.get_n_particles())+",n_pgroups:" + std::to_string(v.get_n_groups())  + "}";
        });
                
	 py::enum_<co::ConfigOutput>(m,"ConfigOutput")
		.value("File", co::ConfigOutput::File)
		.value("Direct", co::ConfigOutput::Direct)
		.export_values();  
		
	 py::enum_<co::ConfigComputation>(m,"ConfigComputation")
		.value("Local", co::ConfigComputation::Local)
		.value("Cluster", co::ConfigComputation::Cluster)
		.export_values();                         
	 
	  py::enum_<co::ErrorCode>(m,"ErrorCode")
		.value("NoError", co::ErrorCode::NoError)
		.value("PathError", co::ErrorCode::PathError)
		.value("ParseError", co::ErrorCode::ParseError)
		.value("OptimizationError", co::ErrorCode::OptimizationError)  
		.value("ComputationError", co::ErrorCode::ComputationError)            
		.export_values();       
 
       
    py::class_<co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>> (m, "EpidemicsEvaluation")
        .def(py::init<std::string, std::string,std::function<void(double, double, const co::EVarManager<co::EFloat64>&, std::vector<co::EFloat64>&, std::vector<co::EFloat64>&,co::ErrorCode&)>>())            
        ;
 
    py::class_<co::ParticleSwarmOptimizer<co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>>> (m, "ParticleSwarmOptimizerEpidemics")
        .def(py::init<const co::PSOOptions&,co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>&>())
        .def("run",&co::ParticleSwarmOptimizer<co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>>::run<co::EFloat64>,py::call_guard<py::gil_scoped_release>())
        ;	
}

