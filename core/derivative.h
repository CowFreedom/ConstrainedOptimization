#pragma once
#include <vector>
#include "options.h"
#include "parameters.h"
#include<iostream>


namespace co{
	
	template<ConfigDerivatives T, class F>
	class Derivative{	
	};
	
	template<class F>
	class Derivative<ConfigDerivatives::FiniteDifferences,F>{
		
		const F delta;
		void(*f)(const std::vector<F>&,const std::vector<F>&,std::vector<F>& result,int stride);
		F(*s)(const std::vector<F>&,const std::vector<F>&);
		public:
		Derivative(void(*_f)(const std::vector<F>&,const std::vector<F>&,std::vector<F>& result,int stride),F(*_s)(const std::vector<F>&,const std::vector<F>&)):f(_f),s(_s),delta(F(0.00001)){}
		Derivative(void(*_f)(const std::vector<F>&,const std::vector<F>&,std::vector<F>& result,int stride),F(*_s)(const std::vector<F>&,const std::vector<F>&), F _delta): delta(_delta), f(_f),s(_s){}
		
		
		/*Get jacobi matrix evaluated at point w*/
		template<class E>
		std::vector<F> get_jacobian(const std::vector<EVarManager<F>>& w, const std::vector<F>& target_times,const std::vector<F>& target_data, std::vector<F>& r_n,F& s_n, E& evaluator){
			//std::vector<F> x_n=evaluator.eval(w,target_times)[0]; //x_n for f(x_n)
			//std::cout<<"Get_jacobian_start\n";
			size_t n_evals=w[0].len();
			std::vector<size_t> multiplier;
	
			const std::vector<EVar<F>>& params=w[0].get_params();
			const std::vector<std::string>& names=w[0].get_names();
			std::vector<EVarManager<F>> evaluations(n_evals);
			size_t mult_counter=0;
			for (size_t i=0;i<n_evals;i++){
				for (size_t j=0;j<n_evals;j++){
					if (j!=i){
						evaluations[i].add(names[j],params[j]);
					}
					else{
						if ((params[i].upper_bound()+delta.get_v())<=params[i].max.get_v()){
							//std::cout<<"plus dif Name: "<<names[i]<<"\n";
							evaluations[i].add(names[i],params[i]+delta);
						}
						else if ((params[i].lower_bound()-delta.get_v())>=params[i].min.get_v()){
							//std::cout<<"Minus dif Name: "<<names[i]<<"\n";
							evaluations[i].add(names[i],params[i]-delta);
							multiplier.push_back(i); //we have to turn the order around if derivative is taken from other side
						}
					}
				}
						
			}
			//std::cout<<"Jacobi: Evaluation creation done\n";
			/*
			std::cout<<"Original values\n";
			for (int i=0;i<w[0].get_params().size();i++){
				std::cout<<w[0].get_names()[i]<<"   "<<w[0].get_params()[i]<<"\n";
			}
			
			std::cout<<"Derivative values\n";
			for (int i=0;i<evaluations[0].get_params().size();i++){
				std::cout<<evaluations[0].get_names()[i]<<"   "<<evaluations[0].get_params()[i]<<"\n";
			}
			*/
			//evaluations.push_back();
			//std::cout<<"Jacobi evals done\n";
			evaluations.push_back(w[0]); //f(x_n) should also be evaluated
			std::vector<std::vector<F>> evals=evaluator.eval(evaluations, target_times, "Evaluating derivative");
			evaluator.send_matrix(evals[0],evals[0].size(),1, "loaded_data");
			//evaluator.send_matrix(evals[1],evals[1].size(),1, "loaded_data_deriv1");
			//std::cout.precision(std::numeric_limits<double>::max_digits10);
			//Now the vector
			//std::cout<<"Length of evals:"<<evals[0].size()<<"\n";
			size_t stride=evals[0].size(); //number of data entries
			std::vector<F> jacobi_matrix(n_evals*stride);
			std::vector<F> res_x_n(stride); //evaluates r(x_n)
			s_n=s(evals[n_evals],target_data); //evaluates original function
			f(evals[n_evals], target_data, res_x_n,0);
			//std::cout<<"Jacobi: All Evals done\n";
			
			//std::cout.precision(std::numeric_limits<double>::max_digits10);
				//std::cout.setf(std::ios_base::fixed,std::ios_base::floatfield);
			//std::cout.precision(9);
			/*
			std::cout<<"Eval length:"<<evals.size()<<"\n";
			std::cout<<"eal s sizes\n";
			for (auto& x:evals){
				std::cout<<x.size()<<"\n";
			}
			*/
			/*
			std::cout<<"\nResult of parsing:\n";
			std::cout<<"x_n\n";
			for(auto& x:evals[n_evals]){
				std::cout<<x<<"\n";
			}
			
			std::cout<<"\n x_ par1:\n";
			for(auto& x:evals[0]){
				std::cout<<x<<"\n";
			}
			
			std::cout<<"\nf(x_n)-y \n";
			
			for(auto& x:res_x_n){
				std::cout<<x<<"\n";
			}
			
			std::cin.get();
			*/
			/*
			std::cout<<"\n x_ par0:\n";
			for(auto& x:evals[n_evals]){
				std::cout<<x<<"\n";
			}
			
			std::cout<<"\n x_ par1:\n";
			for(auto& x:evals[0]){
				std::cout<<x<<"\n";
			}
			
			std::cout<<"\n x_ par2:\n";
			for(auto& x:evals[1]){
				std::cout<<x<<"\n";
			}
			std::cin.get();
			*/
			int iter=0;
			//std::cout<<"bis hier\n";
			//std::cout<<"n_evals:"<<n_evals<<"Stride:"<<stride<<"mutliplier length:"<<multiplier.size()<<"\n";
			if (multiplier.size()>0){
				for (size_t i=0;i<n_evals;i++){
					std::vector<F> res_x_d(stride);
					f(evals[i], target_data, res_x_d,0);
					if (multiplier[iter]!=i){
							for (size_t j=0;j<stride;j++){
								//std::cout<<"Parameter "<<i<<"   "<<res_x_d[j]<<" minus "<<res_x_n[j]<<"\n";
								//std::cout<<"Result: "<<(res_x_d[j]-res_x_n[j])/delta<<"\n";
								//std::cin.get();
								jacobi_matrix[j*n_evals+i]=(res_x_d[j]-res_x_n[j])/delta;
						}
					}
					else{
						for (size_t j=0;j<stride;j++){
							//std::cout<<"Parameter "<<i<<"   "<<res_x_d[j]<<" minus "<<res_x_n[j]<<"  mal minus 1\n";
							//std::cout<<"Result: "<<(res_x_d[j]-res_x_n[j])/delta<<"\n";
							//std::cin.get();
							jacobi_matrix[j*n_evals+i]=F(-1.0)*(res_x_d[j]-res_x_n[j])/delta;
							iter++;
						}
					}
				}				
			}
			else{
				for (size_t i=0;i<n_evals;i++){
				std::vector<F> res_x_d(stride);
				f(evals[i], target_data, res_x_d,0);
				for (size_t j=0;j<stride;j++){
							//std::cout<<"Parameter "<<i<<"   "<<res_x_d[j]<<" minus "<<res_x_n[j]<<"\n";
							//std::cout<<"Result: "<<(res_x_d[j]-res_x_n[j])/delta<<"\n";
							//std::cin.get();
							jacobi_matrix[j*n_evals+i]=(res_x_d[j]-res_x_n[j])/delta;
					}
				}
				
			}

	//std::cin.get();
			r_n=res_x_n;

			
			/*
			std::cout<<"Jacobi matrix:\n";
		
			
				for (size_t j=0;j<stride;j++){
					for (size_t i=0;i<n_evals;i++){
						std::cout<<jacobi_matrix[j*n_evals+i].get_v()<<"  ";
					}
					std::cout<<"\n";
			}
			*/
			return jacobi_matrix;
			
		}
		
	};
	
	
}