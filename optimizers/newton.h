#include "../core/options.h"
#include "../core/parameters.h"
#include <thread>
#include "constrained_optimization.h"
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
				if (sum<=T(0.1)){
					std::cout<<"Has converged true!\n";
					return true;
				}
				else{
					return false;
				}
			}

			template<class T>
			bool run(const std::vector<EVarManager<T>>& initial_params){
			T delta=T(0.0000001);
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
			
			while (run_finished, iter<20){
				std::cout<<"Newton: Starting iteration "<<iter<<"\n";
				std::vector<T> r_n;

				Derivative<ConfigDerivatives::FiniteDifferences,T> deriv(ls::mse); //change the finite differences into something agnostic
				std::vector<T> J=deriv.get_jacobian<E>(parameters, target_times,target_data,r_n,evaluator);

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
				auto& params=parameters[0].get_params();
				for (int i=0;i<j_m;i++){
				/*	std::cout<<"res[i]: "<<res[i]<<" params[i]: "<<params[i].val<<"\n";
					std::cout<<"delta*res[i]: "<<delta.testmult(res[i])<<"\n";
					std::cout<<"params.val+delta*res[i]: "<<params[i].val+delta.testmult(res[i])<<"\n";
					std::cin.get();
				*/
					res[i]=params[i].val+delta*res[i];
				}
				
				
				run_finished=parameters[0].update_parameters_cut(res);
				
				run_finished=has_converged<T>(res);
				iter++;
			}
			
			evaluator.send_parameters(parameters[0], "These are the estimated parameters of the problem.");
			
			size_t num_threads=3;
			return true;
			};
			
		};
	
}