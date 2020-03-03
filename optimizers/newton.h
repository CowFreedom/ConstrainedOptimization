#include "../core/options.h"
#include "../core/parameters.h"
#include <thread>
#include "../core/constrained_optimization.h"
#include "../core/transformation.h"
#include "../core/derivative.h"
#include "loss_functions.h"
#include <limits>
#include <numeric>
//#include<chrono>
//#include <mutex>


namespace co{
 
/*
std::mutex g_display_mutex;
			void f1(){
			std::thread::id id = std::this_thread::get_id();
 
    g_display_mutex.lock();
		std::cout<<"Thread with id "<<id<<"started!\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(4000));	
				  std::thread::id this_id = std::this_thread::get_id();
  //  g_display_mutex.unlock();
			
			
			std::cout<<"Thread with id "<<id<<"finished!\n";			
			}
			*/
			
			

		template<class E>
		class NewtonOptimizer{
			
			private:
			Options options;
			E& evaluator;
			
			template<class T>
			bool update_parameters_stepsize_wolfe_condition(const std::vector<T>& target_data,const std::vector<T>& target_times,const std::vector<T>& J, std::vector<T> d, std::vector<EVarManager<T>>& current_params, T s_n){
				T alpha=T(1.0);
				T c1=(0.0004);
				T c2=(0.1);
				size_t j_n=target_data.size();
				size_t j_m=J.size()/j_n;
				T f_xn=s_n;
				
				std::vector<T> current_res=d;
				size_t iter=0;
				
				while (iter<15){
					std::vector<EVarManager<T>> evaluations=current_params;
					auto& params=evaluations[0].get_params();
					for (int i=0;i<current_res.size();i++){
						current_res[i]=params[i].val+alpha*d[i];
						std::cout<<"d["<<i<<"]:"<<d[i]<<"\n";
					}

					evaluations[0].update_parameters_cut(current_res);	
					
					std::vector<std::vector<T>> evals=evaluator.eval_specific(evaluations, target_times, "wolfe_condition_evaluations/","Wolfe Condition evaluation");
					T f_alpha=ls::mse(evals[0], target_data);
					
					
					std::vector<T> grad_f(j_m,T(0.0));
					for (int i=0;i<j_m;i++){
						for (int j=0;j<j_n;j++){
							grad_f[i]+=J[j*j_m+i];
						}
					}
					std::vector<T> temp(j_m);
					
					dgms<typename std::vector<T>::iterator,T>(d.begin(), temp.begin(),1, j_m, c1*alpha);
					/*
					T f_xn=T(0.0);
					T f_alpha=T(0.0);
					for (size_t i=0;i<j_m;i++){
						f_xn+=s_n[i];
						f_alpha+=s_alpha[i];
					}
					*/
					
					
					T f_sum1=std::inner_product(temp.begin(),temp.end(),grad_f.begin(),f_xn);
					std::cout<<"f_alpha:"<<f_alpha<<"\nf_sum1:"<<f_sum1<<"\n alpha:"<<alpha<<"\nMSE:"<<f_alpha<<"\n";
					std::cin.get();
					if (f_alpha<=f_sum1){
						current_params=evaluations;
						std::cout<<"Done wolfing\n";
						return true;
					}
					
					alpha=T(0.25)*alpha;
					iter++;
				
				}
				
				return false;
			}
			
						template<class T>
			bool update_parameters_stepsize_smaller_var(const std::vector<T>& target_data,const std::vector<T>& target_times,const std::vector<T>& J, std::vector<T> d, std::vector<EVarManager<T>>& current_params, T s_n){
				T alpha=T(1.0);
				T c=T(1.0);
				size_t j_n=target_data.size();
				size_t j_m=J.size()/j_n;
				T f_xn=s_n;
				
				std::vector<T> current_res=d;
				size_t iter=0;
				
				while (iter<15){
					std::vector<EVarManager<T>> evaluations=current_params;
					auto& params=evaluations[0].get_params();
					for (int i=0;i<current_res.size();i++){
						current_res[i]=params[i].val+alpha*d[i];
						std::cout<<"d["<<i<<"]:"<<d[i]<<"\n";
					}

					evaluations[0].update_parameters_cut(current_res);	
					
					std::vector<std::vector<T>> evals=evaluator.eval_specific(evaluations, target_times, "stepsize_smaller_var_evaluations/","Stepsize evaluation");
					T f_alpha=ls::mse(evals[0], target_data);
					
					
					std::cout<<"New MSE:"<<f_alpha<<"\nOld MSE:"<<s_n<<"\n";
					
					if (f_alpha<=c*s_n){
						current_params=evaluations;
						std::cout<<"Done stepsize smaller var\n";
						return true;
					}
					
					alpha=T(0.25)*alpha;
					iter++;
				
				}
				
				return false;
			}
			
			template<class T>
			bool update_parameters_stepsize_random(std::vector<EVarManager<T>>& parameters, std::vector<T> d){
			
				T norm=T(0.0);
				for (auto& x: d){
					norm+=(x*x).sqrt();
				}
				T alpha=T(1.0)/norm;
				auto& params=parameters[0].get_params();
				
				for (int i=0;i<d.size();i++){
				/*	std::cout<<"res[i]: "<<res[i]<<" params[i]: "<<params[i].val<<"\n";
					std::cout<<"delta*res[i]: "<<delta.testmult(res[i])<<"\n";
					std::cout<<"params.val+delta*res[i]: "<<params[i].val+delta.testmult(res[i])<<"\n";
					std::cin.get();
					*/
					
					//std::cout<<"params[i]: "<<params[i]<<"\n";
					//std::cout<<"res[i]:"<<res[i]<<"\n";
					//std::cout<<"params[i]+delta*res[i] "<<delta*res[i]<<"\n";
					d[i]=params[i].val+alpha*d[i];
				}
					
					
				bool res=parameters[0].update_parameters_cut(d);				
				
				return res;
			}
			
			public:
			NewtonOptimizer(const Options& _options, E& _evaluator):options(_options), evaluator(_evaluator){
				
			}
			
			//Checks if zero is found. In this case, a zero is found if the sum of /delta w is smaller than a threshold
			template<class T>
			bool has_converged(const std::vector<T>& res){
				T sum=T(0.0);
				
				for (auto& x:res){
					sum+=x.abs();
				}
				std::cout<<"Has converged sum:"<<sum<<"\n";
				if (sum<=T(0.00001)){
					std::cout<<"Has converged true!\n";
					return true;
				}
				else{
					return false;
				}
			}

			template<class T>
			bool run(const std::vector<EVarManager<T>>& initial_params){
			T delta=T(0.0001);
			std::cout<<"Newton Optimizer started\n";
			//load target data
			std::vector<T> target_data;
			std::vector<T> target_times;
			evaluator.load_target(target_times,target_data);
			std::vector<EVarManager<T>> parameters=initial_params;
			//evaluate initial parameters
			size_t j_n=target_data.size(); //height of jacobi matrix
			size_t j_m=parameters[0].len(); //length of jacobi matrix
			//x_n=evaluator.eval(initial_params,target_times)[0];
			size_t iter=0;
			bool run_finished=false;
			
			while ((run_finished==false )&& iter<20){
				std::cout<<"Newton: Starting iteration "<<iter<<"\n";
				std::vector<T> r_n;
				T s_n;
			
				Derivative<ConfigDerivatives::FiniteDifferences,T> deriv(ls::err1,ls::mse); //change the finite differences into something agnostic
				std::vector<T> J=deriv.get_jacobian(parameters, target_times,target_data,r_n,s_n,evaluator);

				evaluator.send_matrix(J,j_n,j_m, "jacobi_matrix"); //print Jacobi matrix (likely to file)
				
				std::vector<T> J_T(j_n*j_m); //transpose of Jacobi matrix
				transpose(J.begin(),J_T.begin(),j_n,j_m); //fill J_T with values
				evaluator.send_matrix(J_T,j_m,j_n, "jacobi_transpose");
				//evaluator.send_matrix(J_T,j_m,j_n, "jacobi_transpose");
				std::vector<T> J_T_J(j_m*j_m);
				mul::dgemm_nn(j_m,j_m,j_n,T(1.0),J_T.begin(),1,j_n,J.begin(),1,j_m,T(0.0),J_T_J.begin(),1,j_m);
				evaluator.send_matrix(J_T_J,j_m,j_m, "jacobi_transpose_dot_jacobi");
			
				std::cout.precision(std::numeric_limits<double>::max_digits10);
				/*
				for (int i=0;i<j_m;i++){
					for (int j=0;j<j_m;j++){
						std::cout<<J_T_J[i*j_m+j].get_v()<<"   ";
					}
					std::cout<<"\n";
				}
				*/
				
				/*Optional start: Create inverse matrix to calculate covariance matrix*/
				std::vector<T> J_T_J_inv(j_m*j_m);
				dc::inverse_square_qr<T>(J_T_J.begin(), J_T_J_inv.begin(), j_m);
				evaluator.send_matrix(J_T_J_inv,j_m,j_m, "jacobi_transpose_dot_jacobi_inverse");
				T sigma=std::inner_product(r_n.begin(),r_n.end(),r_n.begin(),T(0.0))/(T(j_n-j_m));
				dgms<typename std::vector<T>::iterator,T>(J_T_J_inv.begin(), J_T_J_inv.begin(),j_m, j_m, sigma);
				evaluator.send_matrix(J_T_J_inv,j_m,j_m, "jacobi_transpose_dot_jacobi_inverse_scaled");
				
				//bis hier wurde kontrolliert
				/*Optional end*/
		
				//Calculate R\deltatheta=-q1
				std::vector<T> Qt(j_n*j_n);
				std::vector<T> R(j_n*j_m);
				dc::qr<typename std::vector<T>::iterator,T>(J.begin(), j_n, j_m, Qt.begin(), R.begin());
								
				
			//	std::cout<<"nach qr\n";
				std::vector<T> q1(j_n);
				co::mul::dgemm_nn(j_n,1,j_n,T(1.0),Qt.begin(),1,j_n,r_n.begin(),1,1,T(0.0),q1.begin(),1,1); 
			//	std::cout<<"nach mult\n";
				std::vector<T> res(j_m);
				/*
				std::cout<<"\nq1\n";
				for (auto& x:q1){
					std::cout<<x<<"   ";
				}
				

				std::cout<<"\nQt=\n";
				//transpose(Qt.begin(),Qt.begin(),j_n,j_n);
				for (int i=0;i<j_n;i++){
					for (int j=0;j<j_n;j++){
						std::cout<<Qt[i*j_n+j].get_v()<<"   ";
					}
					std::cout<<"\n";
				}
				std::cout<<"\nR=\n";
				for (int i=0;i<j_n;i++){
					for (int j=0;j<j_m;j++){
						std::cout<<R[i*j_m+j].get_v()<<"   ";
					}
					std::cout<<"\n";
				}
				
				*/
				
				dgms<typename std::vector<T>::iterator,T>(q1.begin(), q1.begin(),j_m, 1, T(-1.0)); //q1=-q1
				/*
				std::cout<<"\nr_n\n";
				for (auto& x:r_n){
					std::cout<<x<<"   ";
				}
					std::cout<<"\n-q1\n";
				for (auto& x:q1){
					std::cout<<x<<"   ";
				}
				
				*/
				dc::backwards_substitution<T>(R.begin(),res.begin(), 1,q1.begin(), j_m);
				/*
				std::cout<<"\res\n";
				for (auto& x:res){
					std::cout<<x<<"   ";
				}
				
				std::cout<<"Stop\n";
				std::cin.get();
				*/
				/*std::cout<<"q1:\n";
				
				for (auto x:q1){
					std::cout<<x<<"  ";
				}
				*/
				//Update parameters
				//std::cout<<"Parameter adjustment\n";
			
			run_finished=has_converged<T>(res);
				
			//Update parameters only if run is still ongoing
			
				//remove here start
				
			
			
			//std::cout<<"Wolfe condition comes now:\n";
			//update_parameters_stepsize_wolfe_condition(target_data,target_times,J,res,parameters,s_n);
			update_parameters_stepsize_smaller_var(target_data,target_times,J,res,parameters,s_n);
			//std::cout<<"Wolfe over\n";
			//update_parameters_stepsize_random(parameters,res);

				
				
				
				std::cout<<"Run has finished: "<<run_finished<<"iteration:"<<iter<<"\n";
				iter++;
			}
						
			evaluator.send_parameters(parameters[0], "These are the estimated parameters of the problem.");
			
			size_t num_threads=3;
			return true;
			};
			
		};
		
	
}