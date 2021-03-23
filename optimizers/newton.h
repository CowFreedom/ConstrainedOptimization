/** \file newton.h
 *The Gauss Newton procedure minimizes functions that can be represented as sums of squares. 
 *While the implementation is generic and straightforward, instances of evaluation.h heavily 
 *steer the way the actual compuation is done. 
 *Currently three line search methods are implemented.
 */

#include "../core/options.h"
#include "../core/parameters.h"
#include "../core/constrained_optimization.h"
#include "../core/transformation.h"
#include "../core/derivative.h"
#include "loss_functions.h"
#include <limits>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <vector>

namespace co{
 

			
		
			
		/**  
		 Creates a string that stores information about the current Newton iteration.
		 This information includes the values of the currently estimated parameters, error measures and 
		 covariances. Because the number of digits saved in the string heavily depends on the width of the underlying numeric datatype, 
		 the function is templatized.
		 This entails that wider types save more digits while shorter types are stored with less precision.
		 @param[in] vars The status of the variables of the current Newton iteration.
		 @param[in] J_T_J_inv_scaled Inverse of first order Hessian approximation multiplied by the standard deviation. Serves as approximation to Covariance Matrix.
		 @param[in] iteration Current Newton iteration
		 @param[in] squared_error Estimated of squared error between the experimental data points and the simulated data points.
		 \return String containing formatted information about the current optimization iteration
		 */
		template<class T>
		std::string print_info(const EVarManager<T>& vars,const std::vector<T>& J_T_J_inv_scaled, size_t iteration, T squared_error);
			
		/**  
		 Creates a string that stores information about the current Newton iteration for the EFloat64 datatype.
		 This information includes the values of the currently estimated parameters, error measures and 
		 covariances. Because EFloat64's underlying value type is a double, 15 digits of precision are saved.
		 Please note that this does not mean that all 15 digits are reliable. Look at the low and high error bounds of the datatype
		 to get an estimate of its true value.
		 @param[in] vars The status of the variables of the current Newton iteration.
		 @param[in] J_T_J_inv_scaled Inverse of first order Hessian approximation multiplied by the standard deviation. Serves as approximation to Covariance Matrix.
		 @param[in] iteration Current Newton iteration
		 @param[in] squared_error Estimated of squared error between the experimental data points and the simulated data points
		 \return String containing formatted information about the current optimization iteration
		 */
		template<>
		std::string print_info<EFloat64>(const EVarManager<EFloat64>& vars,const std::vector<EFloat64>& J_T_J_inv_scaled, size_t iteration, EFloat64 squared_error){
			std::ostringstream res;
			res.precision(15); //dependent on type
			
			size_t m=vars.len();
			const std::vector<EVar<EFloat64>>& params=vars.get_params();
			const std::vector<std::string>& names=vars.get_names();
			
			std::vector<double> std_devs_inv(m,0.0);
			
			//Calculate variances for each parameter
			for (int i=0;i<m;i++){
				std_devs_inv[i]=1.0/sqrt(J_T_J_inv_scaled[i+i*m].get_v());
			}
			
			res<<std::setw(30)<<std::left<<"Newton iteration: "<<iteration<<"\n";
			res<<std::setw(30)<<std::left<<"Squared Error: "<<squared_error.get_v()<<"\n";
			res<<std::setw(30)<<std::left<<"Parameter"<<"|"<<std::setw(25)<<"Estimate"<<"|"<<std::setw(30)<<"Approx. Standard Error"<<"|"<<std::setw(20)<<"Approx. Correlation Matrix\n";
			for (int i=0;i<m;i++){
				res<<std::setw(30)<<std::left<<names[i]<<"|"<<std::setw(25)<<std::right<<params[i].get_value_as_double()<<"|"<<std::setw(30)<<sqrt(J_T_J_inv_scaled[i+i*m].get_v())<<"|";
				for (int j=0;j<=i;j++){
					res<<"\t"<<std::setw(13)<<std_devs_inv[i]*std_devs_inv[j]*J_T_J_inv_scaled[j+i*m].get_v();
				}
				res<<"\n";
			}
			
			return res.str();
			
		}			
		/*! This class represents an instance of the Newton Gauss algorithm. Instances of this class must be created if one wants to run the Newton Gauss algorithm.
		Instances of the Evaluator class from evaluator.h steer the computation behavior while instances of co::Options internally configure the class.*/
		template<class E>
		class NewtonOptimizer{
			
			private:
			NewtonOptions options;
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
						//std::cout<<"d["<<i<<"]:"<<d[i]<<"\n";
					}

					evaluations[0].update_parameters_cut(current_res);	
					ErrorCode eval_error;
					std::vector<std::vector<T>> evals=evaluator.eval_specific(evaluations, target_times,"wolfe_condition_evaluations/", eval_error,"Wolfe Condition evaluation");
					if(eval_error!=ErrorCode::NoError){
						std::cout<<"Error evaluating the target function\n";
						return false;
					}
					T f_alpha=evaluator.s(evals[0], target_data);
					
					
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
					//std::cout<<"f_alpha:"<<f_alpha<<"\nf_sum1:"<<f_sum1<<"\n alpha:"<<alpha<<"\nMSE:"<<f_alpha<<"\n";
					//std::cin.get();
					if (f_alpha<=f_sum1){
						current_params=evaluations;
						saved_losses_in_past_iteration.push_back((double)s_n);
						std::cout<<"Done wolfing\n";
						return true;
					}
					
					alpha=T(0.25)*alpha;
					iter++;
				
				}
				
				return false;
			}
			
			template<class T>
			bool update_parameters_stepsize_smaller_var(const std::vector<T>& target_data,const std::vector<T>& target_times,const std::vector<T>& J, std::vector<T> d, std::vector<EVarManager<T>>& current_params, T& s_n,T& e_g2,std::string& stepsize_infos){
				T alpha=T(options.get_stepsize_alpha());
				T c=T(1.0);
				size_t j_n=target_data.size();
				size_t j_m=J.size()/j_n;
				int n_threads=(NTHREADS_SUPPORTED-1>0)?(NTHREADS_SUPPORTED-1):1;
				T scale=T(0.7);
				T eps=T(1e-10);
			
				std::vector<T> alphas(n_threads);
				size_t iter=0;
				std::cout<<"\n****Estimating stepsize****\n";
				T lam=T(options.get_stepsize_decay());
				T d2=T(0.0);
				for (int i=0;i<d.size();i++){
					d2+=d[i]*d[i];
				}
				//In the first iteration, we do not have any previous descent magnitude to refer to
				if (e_g2==T(0.0)){
					e_g2=lam*d2;
				}
 				e_g2=lam*e_g2+(T(1.0)-lam)*d2;
				
				int frac=120/n_threads;
				int max_iter=(frac>=1)?frac:1;
				
				while (iter<10){
					//std::vector<EVarManager<T>> evaluations=current_params;
					std::vector<EVarManager<T>> evaluations;
					for (int i=0;i<n_threads;i++){
						
						T factor=(alpha/(e_g2+eps).sqrt());
						alphas[i]=factor;
						std::vector<T> current_res=d;
						evaluations.push_back(current_params[0]);
						auto& params=evaluations[i].get_params();
						for (int i=0;i<current_res.size();i++){
							
							current_res[i]=params[i].val+factor*d[i];
						//std::cout<<"d["<<i<<"]:"<<d[i]<<"\n";
						}
						evaluations[i].update_parameters_cut(current_res);	
						alpha=scale*alpha;
					}
					
					ErrorCode eval_error;
					std::vector<std::vector<T>> evals=evaluator.eval_specific(evaluations, target_times,"stepsize_smaller_var_evaluations/", eval_error,"Stepsize evaluation");
					if(eval_error!=ErrorCode::NoError){
						std::cout<<"Error evaluating the target function\n";
						return false;
					}
					std::vector<T> f_alphas(n_threads);
					std::ostringstream output;
					output.precision(15); //dependent on type
					output<<"Old SE:"<<s_n<<"\n";
					output<<"E[g2]="<<e_g2<<"\n";
					output<<"g2="<<d2<<"\n";
					output<<std::setw(30)<<std::left<<"Stepsize"<<"|"<<std::setw(30)<<"New Squared Error"<<"| \n";
					T f_alpha=T(std::numeric_limits<double>::max());
					int best_alpha=0;
					for (int i=0;i<n_threads;i++){
						T f_alpha_temp=evaluator.s(evals[i], target_data);
						output<<std::setw(30)<<std::left<<(double) alphas[i]<<"|"<<std::setw(30)<<f_alpha_temp<<"|"<<"\n";
						if (f_alpha_temp<=f_alpha){
							f_alpha=f_alpha_temp;
							best_alpha=i;
						}
					}
					
					output<<"\n";
					output<<"Best SE:"<<f_alpha<<"\nOld SE:"<<s_n<<"****\n";

					stepsize_infos+=output.str();
					std::cout<<output.str();
					if (f_alpha<=c*s_n){
						s_n=f_alpha;
						saved_losses_in_past_iteration.push_back((double)s_n);
						std::vector<EVarManager<T>> res;
						res.push_back(evaluations[best_alpha]);
						current_params=res;
						std::cout<<"Stepsize Finder: Done (new SE smaller than old MSE)\n";
						std::cout<<"****Estimating stepsize done ****\n";
						return true;
					}
					
					alpha=scale*alpha;
					iter++;
				
				}
				std::cerr<<"No valid stepsize could be found within a limited number of iterations\n";
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
			
			double convergence_threshold = 0.0001;
			
			public:
			/*! Creates the class.
			@param[in] _options Configures the Newton Gauss optimizer internally, such as choosing the derivative evaluation type (e.g. Finite Differences) and line search method.
			@param[in] _evaluator Steers how data is loaded and evaluated (e.g. parsed from file, given from within UG4) */
			NewtonOptimizer(const NewtonOptions& _options, E& _evaluator):options(_options), evaluator(_evaluator){
				
			}

			/*! Checks if the Newton iterations have converged to a root. This is equal to having a descent direction whose magnitude is close to zero.
			@param[in] res Value of the descent direction of the function to be evaluated. 
			\return A bool indicating if iterations have converged.
			*/
			template<class T>
			bool has_converged(const std::vector<T>& res){
				T sum=T(0.0);
				
				for (auto& x:res){
					sum+=x*x;
					//std::cout<<x<<"\n";
					//std::cin.get();
				}
				std::cout<<"Squared L2 norm of descent direction:"<<sum<<"\n";
				if (sum<=T(convergence_threshold)){
					std::cout<<"Has converged true!\n";
					return true;
				}
				else{
					return false;
				}
			}
			/*! Runs Gauss Newton's algorithm. Only this function has to be called to run the complete procedure.
			@param[in] initial_params Initial parameters containing starting values for the procedure.
			@param[in] estimated_parameters This will save the estimated parameters of the problem
			\return Code indicating success or failure of running the Newton procedure.
			*/
			template<class T>
			ErrorCode run(const EVarManager<T>& initial_params, EVarManager<T>& estimated_parameters){
				saved_losses_in_past_iteration=std::vector<double>();
				T delta=T(0.0001);
				//std::cout<<"Newton Optimizer started\n";
				//load target data
				std::vector<T> target_data;
				std::vector<T> target_times;
				ErrorCode load_code=evaluator.load_target(target_times,target_data); //load target vector
				if (load_code!=ErrorCode::NoError){
					std::cerr<<"Error loading target data!\n";
					return load_code;
				}
				
				std::vector<EVarManager<T>> parameters;
				parameters.push_back(initial_params);

				//evaluate initial parameters
				size_t j_n=target_data.size(); //height of jacobi matrix
				size_t j_m=parameters[0].len(); //length of jacobi matrix
				//x_n=evaluator.eval(initial_params,target_times)[0];
				size_t iter=0;
				bool run_finished=false;
				T e_g2=T(0.0); ;//adagrad like adaptive stepsize factor
		
				while ((run_finished==false )&& iter<4000){
					std::cout<<"Newton: Starting iteration "<<iter<<"\n";
					std::vector<T> r_n;
					T s_n;
					Derivative<ConfigDerivatives::FiniteDifferences,T> deriv(ls::err1,ls::mse); //change the finite differences into something agnostic
					ErrorCode eval_error;
					std::vector<T> J=deriv.get_jacobian(parameters, target_times,target_data,r_n,s_n,evaluator,eval_error);
					if(eval_error!=ErrorCode::NoError){
						return eval_error;
					}

					evaluator.send_matrix(J,j_n,j_m, "jacobi_matrix"); //print Jacobi matrix (likely to file)
					
					std::vector<T> J_T(j_n*j_m); //transpose of Jacobi matrix
					transpose(J.begin(),J_T.begin(),j_n,j_m); //fill J_T with values
					//evaluator.send_matrix(J_T,j_m,j_n, "jacobi_transpose");
					//evaluator.send_matrix(J_T,j_m,j_n, "jacobi_transpose");
					std::vector<T> J_T_J(j_m*j_m);
					mul::dgemm_nn(j_m,j_m,j_n,T(1.0),J_T.begin(),1,j_n,J.begin(),1,j_m,T(0.0),J_T_J.begin(),1,j_m);
					//evaluator.send_matrix(J_T_J,j_m,j_m, "jacobi_transpose_dot_jacobi");
				
					std::cout.precision(std::numeric_limits<double>::max_digits10);
					
					/*Optional start: Create inverse matrix to calculate covariance matrix*/
					std::vector<T> J_T_J_inv(j_m*j_m);
					dc::inverse_square_qr<T>(J_T_J.begin(), J_T_J_inv.begin(), j_m);
					//evaluator.send_matrix(J_T_J_inv,j_m,j_m, "jacobi_transpose_dot_jacobi_inverse");
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
					
					dgms<typename std::vector<T>::iterator,T>(q1.begin(), q1.begin(),j_m, 1, T(-1.0)); //q1=-q1

					dc::backwards_substitution<T>(R.begin(),res.begin(), 1,q1.begin(), j_m);
	
					//Update parameters
					//std::cout<<"Parameter adjustment\n";
				
					run_finished=has_converged<T>(res);
						
					//Update parameters only if run is still ongoing

					
					//initialize adaptive step size

					
					//std::cout<<"Wolfe condition comes now:\n";
					//update_parameters_stepsize_wolfe_condition(target_data,target_times,J,res,parameters,s_n);
					std::string stepsize_infos;
					bool success=update_parameters_stepsize_smaller_var(target_data,target_times,J,res,parameters,s_n,e_g2,stepsize_infos);
					if (success!=true){
						break;
					}
					
					evaluator.send_info(stepsize_infos,"summary_of_stepsize_estimation");
					//std::cout<<"Wolfe over\n";
					//update_parameters_stepsize_random(parameters,res);
					std::string infos=print_info<T>(parameters[0],J_T_J_inv, iter,s_n);
					evaluator.send_info(infos,"summary_of_estimation");
					std::cout<<infos<<"\n";
					
					
					
					//std::cout<<"Run has finished: "<<run_finished<<"iteration:"<<iter<<"\n";
					iter++;
				}

			if (run_finished){	
				evaluator.send_parameters(parameters[0], "These are the estimated parameters of the problem.");
				estimated_parameters=parameters[0];
				return ErrorCode::NoError;
			}
			else{
				estimated_parameters=parameters[0];
				return ErrorCode::OptimizationError;
			}
			}
			
			std::vector<double> saved_losses_in_past_iteration;
			
			std::vector<double> get_saved_losses_in_past_iteration_as_double() const{
				return saved_losses_in_past_iteration;
			}
			
			double get_convergence_threshold() const{
				return convergence_threshold;
			}
			
			void set_convergence_threshold(double input) {
				convergence_threshold = input ;
			} 
		
		};
		/** \example newton_example.cpp
		 * This is an example on how to use the Newton-Gauss solver.
		 */		
	
}
