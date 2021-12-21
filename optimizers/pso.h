/** \file pso.h
 *The Particle Swarm Optimization (PSO) procedure minimizes functions.
 *An explanation can be found in "Parameter Estimation in Ordinary Differential Equations Modeling via Particle Swarm Optimization"
 *by Devin Akman et al.
 *The algorithm scales well to multiple processing cores and is a useful precursor to find initial
 *values for the Gauss Newton algorithm.
 */

#include "../core/options.h"
#include "../core/parameters.h"
#include "../core/constrained_optimization.h"
#include "../core/transformation.h"
#include "../core/derivative.h"
#include "loss_functions.h"
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <stdexcept>


#ifndef DEBUG
    #define DEBUG 0
#endif

namespace co{

        
    template <class E>
    class ParticleSwarmOptimizer{
        
        private:
        int n_particles;
        int n_groups;
        int max_iterations;
        PSOOptions options;
        E& evaluator;

        double particle_w=0.3;
        double particle_c=1;
        
        template<class T>
        class Particle{
            private:
            int id;
            int group_id;
            T c=T(1); // displacement scaling factor
            T w=T(0.3);// velocity scaling factor
    
            public:
            std::vector<T> position;
            std::vector<T> velocity;
            
            std::vector<T> personal_best_position;
            T personal_best_fitness;

            Particle() =delete;
            
            Particle(E& evaluator, const std::vector<T>& target_data, std::vector<T> initial_position, const std::vector<T>& initial_value, int _id, double _c, double _w):id(_id), c(_c), w(_w){
                position=initial_position;
                personal_best_position=initial_position;
                
                personal_best_fitness=evaluator.s(target_data,initial_value);
                
                /*
                
                std::cout<<"\n";
                std::cout<<"Personal fitness:"<<personal_best_fitness.get_v()<<"\n";
                std::cin.get();
                */
                //initialize velocity
                int n=initial_position.size();
                velocity=std::vector<T>(n,T(0.0));
                
            }
    
            void update_personal_best_fitness(E& evaluator,const std::vector<T>& target_data, const std::vector<T>& f_value, std::vector<T>& local_fitness,std::vector<std::vector<T>>& local_best_position,std::vector<int>& best_id){
                
                    T current_personal_fitness=evaluator.s(target_data,f_value);

                    if (current_personal_fitness<=personal_best_fitness){
                        
                        personal_best_fitness=current_personal_fitness;
                        personal_best_position=position;
                        if (current_personal_fitness<=local_fitness[group_id]){
                            local_fitness[group_id]=current_personal_fitness;
                            local_best_position[group_id]=position;
                            best_id[group_id]=id;
                        }
                    }
            }
            
            bool update_direction(const std::vector<T>& target_data, const std::vector<T>& bounds, EVarManager<T>& evaluation,const std::vector<std::string>& param_names,const std::vector<T>& local_best_position){
                int n=position.size();
                std::vector<T> g(n);
                double radius=0.0;
                //std::cout<<"n:"<<n<<"\n";
                //std::cout<<"personal best n"<<personal_best_position.size()<<"\n";
                //std::cout<<"local best_position n"<<local_best_position.size()<<"\n";
                T divisor=T(1.0)/T(3.0);
                bool schranke =true;
                for (int i=0;i<n;i++){
                    if (personal_best_position[i]!=local_best_position[i]){
                        schranke=false;
                        break;
                    }
                }
                
                if (schranke){
                    divisor=T(1.0)/T(4.0);
                }
        
                for (int i=0;i<n;i++){
                    /*
                    std::cout<<"Playbooy\n";
                    std::cout<<n<<"\n";
                    std::cout<<position[i]<<"\n";
                    std::cout<<personal_best_position[i]<<"\n";
                    std::cout<<local_best_position[i]<<"\n";
                    */
                    g[i]=position[i]+c*(personal_best_position[i]+local_best_position[i]-T(2.0)*position[i])*divisor;
                    //std::cout<<"pb: "<<personal_best_position[i].get_v()<<" lp: "<<local_best_position[i].get_v()<<"\n";
                    radius+=(double)((position[i]-g[i])*(position[i]-g[i]));
                }

            //    for (int i=0;i<n;i++){
            //        std::cout<<"g["<<i<<"]:"<<g[i]<<"\n";
            //    }
                radius=std::sqrt(radius);
            //    std::cin.get();
                //std::cout<<"loop ende\n";
                std::random_device rd;  //Will be used to obtain a seed for the random number engine
                std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
                std::vector<T> position_new(n);
        //    std::cout<<"Id:"<<id<<" with radius: "<<sqrt(radius)<<"\n";
                std::uniform_real_distribution<> dis(-radius, radius); //suboptimal, n dimensional normal distribution generated
                for (int i=0;i<n;i++){    
                    position_new[i]=g[i]+T(dis(gen)); //b
                    //std::cout<<radius<<"\n";
                
                    velocity[i]=w*velocity[i]+position_new[i]-position[i]; //c
                    position[i]=position[i]+velocity[i]; //d
                    //std::cout<<position[i]<< " bounds l "<<bounds[2*i]<<" bounds h "<<bounds[2*i+1]<<"\n";
                    //e
                    
                
                    if ((double) position[i]<=(double) bounds[2*i]){
                        position[i]=bounds[2*i];
                        velocity[i]*=T(-1.0);
                    }
                    else if((double) position[i]>(double) bounds[2*i+1]){
                        position[i]=bounds[2*i+1];
                        velocity[i]*=T(-1.0);
                    }
                //    std::cout<<"New position["<<i<<"]="<<position[i]<<" bounds low"<<bounds[2*i].get_v()<< "bounds high"<<bounds[2*i+1].get_v()<<"\n";
                //    std::cin.get();
                    EVar<T> val(position[i],bounds[2*i],bounds[2*i+1]);
                    evaluation.add(param_names[i],val);
                }
                /*
            std::cout<<"new position for id " <<id<<"\n";
                    for (auto& x: position){
                        std::cout<<(double) x.get_v()<<"  ";
                    }
                    std::cout<<"\n";
            std::cout<<"velocity for id " <<id<<"\n";
                    for (auto& x: velocity){
                        std::cout<<(double) x.get_v()<<"  ";
                    }
                    std::cout<<"\n";
                    */
                /*std::cout<<"\n -\n";
                for (auto& x:position){
                    std::cout<<x.get_v()<<"   ";
                }
                std::cout<<"\n";
                */
                return true;    
            }

        void update_direction_random(const std::vector<T>& bounds, EVarManager<T>& evaluation,const std::vector<std::string>& param_names){        
                std::random_device rd;  //Will be used to obtain a seed for the random number engine
                std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
                for (int i=0;i<param_names.size();i++){
                    //std::uniform_real_distribution<> dis((double)bounds[2*i],(double) bounds[2*i+1]);        
                    double radius=0.1;
                    
                    if (radius<(double)bounds[2*i]){
                        radius=(double) bounds[2*i];
                    }
                    else if (radius>(double)bounds[2*i+1]){
                        radius=(double) bounds[2*i+1];
                    }
                    
                    std::uniform_real_distribution<> dis(-radius,radius);    
                    position[i]=position[i]+T(dis(gen));
                    EVar<T> val(position[i],bounds[2*i],bounds[2*i+1]);
                    evaluation.add(param_names[i],val);
                }        
        }
            
            void print_fitness() const{
                std::cout<<"my id: "<<id<<" my group id: "<<group_id<<" personal_best_fitness: "<<personal_best_fitness<<"\n";
            }
            
            void set_id(int _id){
                id=_id;
            }
            
            void set_group_id(int id){
                group_id=id;
            }
            T get_personal_best_fitness() const{
                return personal_best_fitness;
            }
            int get_id() const{
                return id;
            }
            int get_group_id() const{
                return group_id;
            }
            
            std::vector<T> get_personal_best_position() const{
                return personal_best_position;
            }
            
            std::vector<T> get_position() const{
                return position;
            }

            
        };
        
        double me;
            
        template<class T>
        std::string print_info(const std::vector<Particle<T>>& particles, const std::vector<std::string>& param_names, int iteration,const std::vector<int>& best_id, const std::vector<T>& local_fitness, const std::vector<std::vector<T>>& local_best_position){
            std::ostringstream res;
            res.precision(12); //dependent on type
    
            
            res<<std::setw(30)<<std::left<<"Particle Swarm Optimization iteration: "<<iteration<<"\n";
            T minimum_error=T(std::numeric_limits<double>::max());
            int minimum_group=-1;

            for (int i=0;i<n_groups;i++){
    
                if(best_id[i]!=-1){            
                    res<<std::setw(30)<<std::left<<"Particle group "<<i<<"\n";
                    res<<std::setw(30)<<std::left<<"Local fitness: "<<std::pow(local_fitness[i].get_v(),0.5)<<"\n";
                    res<<std::setw(30)<<std::left<<"Parameter"<<"|"<<std::setw(25)<<"Estimate"<<"\n";
                    std::vector<T> pos=local_best_position[i];
                    for (int j=0;j<param_names.size();j++){
                        res<<std::setw(30)<<std::left<<param_names[j]<<"|"<<std::setw(25)<<pos[j].get_v()<<"\n";
                    }
                    if (local_fitness[i]<=minimum_error){
                        minimum_error=local_fitness[i];
                        minimum_group=particles[best_id[i]].get_group_id();
                    }
                }
                res<<"\n";
            }
            
            res<<std::setw(30)<<std::left<<"Best group: "<<minimum_group<<"\n";
            res<<std::setw(30)<<std::left<<"Best particle id: "<<best_id[minimum_group]<<"\n";
            res<<std::setw(30)<<std::left<<"Minimum Error: "<<std::pow(minimum_error.get_v(),0.5)<<"\n";        
            me=(double) minimum_error.get_v();        
            return res.str();
        }    
        
        template<class T>
        ErrorCode initialize_positions(const std::vector<std::string> param_names, const std::vector<T>& bounds, const std::vector<T>& target_times, const std::vector<T>& target_data, std::vector<Particle<T>>& particles
        ,std::vector<std::vector<int>>& groups, std::vector<T>& local_best_fitness, std::vector<std::vector<T>>& local_best_position,std::vector<int>& best_id){
            
            int n=param_names.size();
            
            std::vector<EVarManager<T>> evaluations(n_particles);
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
            std::vector<std::vector<T>> initial_position(n_particles);
            //std::cout<<"n:"<<n<<"\n";
        
            for (int i=0;i<n_particles;i++){
                for (int j=0;j<n;j++){
        
                    T low=bounds[2*j];
                    T high=bounds[2*j+1];
                    std::uniform_real_distribution<> dis((double) low, (double) high);
                    T val(dis(gen));
                    initial_position[i].push_back(val);
                    EVar<T> var=EVar<T>(val, low, high);
                    evaluations[i].add(param_names[j],var);
                }
            }

            #if(DEBUG)
                std::cout << "initialize_positions debug print3 post for" << std::endl;
                std::cout << "initialize_positions debug print3.1" << std::endl;
            #endif
            ErrorCode e;
    
            std::vector<Particle<T>> _particles;

            std::vector<std::vector<T>> evals=evaluator.eval(evaluations, target_times, e,"Initializing Particles");
            
            evaluator.send_matrix(evals[n_particles-1],evals[n_particles-1].size(),1, "loaded_data");

            if (e==ErrorCode::NoError){
                for (int i=0; i<n_particles;i++){

                    Particle<T> p(evaluator, target_data,initial_position[i] ,evals[i],i,particle_w, particle_c);
                    //e=p.update_position(evals[i]);
                    _particles.push_back(p);
                    //std::cout<<particles[i].get_id()<<"\n";
                }
            }
            else{
                return e;
            }

            #if(DEBUG)
                std::cout << "initialize_positions debug print4 pre for" << std::endl;
            #endif

            std::uniform_int_distribution<int> dist(0,n_groups-1);
            for (int i=0;i<n_particles;i++){
                int id=dist(rd);
                _particles[i].set_group_id(id);

            }

            #if(DEBUG)
                std::cout << "initialize_positions debug print4 post for" << std::endl;
            #endif

            std::vector<T> p_min(n_groups,T(std::numeric_limits<double>::max()));
            std::vector<std::vector<T>> lp_pos(n_groups);//local_best_position
            std::vector<int> min_id(n_groups);
            //std::cout<<"lp_pos size: "<<lp_pos.size();
            //initialize local fitness

            #if(DEBUG)
                std::cout << "initialize_positions debug print5 pre for" << std::endl;
            #endif

            for (int i=0;i<n_particles;i++){
                int group_id=_particles[i].get_group_id();
                T current_fitness=_particles[i].get_personal_best_fitness();
                if (current_fitness<=p_min[group_id]){
                    p_min[group_id]=current_fitness;
                    best_id[group_id]=_particles[i].get_id();
                    min_id[group_id]=i;
                    lp_pos[group_id]=_particles[i].get_position();
                }
            }

            #if(DEBUG)
                std::cout << "initialize_positions debug print5 post for" << std::endl;
            #endif

            local_best_fitness=p_min;
            local_best_position=lp_pos;
                        
            particles=_particles;
            return e;
        //    std::vector<std::vector<F>> evals=evaluator.eval(evaluations, target_times, e,"Evaluating derivative");
        }
        
        
    
        
        
        template<class T>
        bool has_converged(std::vector<T> local_fitness){
            T f_min=local_fitness[0];
            for (int i=1;i<local_fitness.size();i++){
                if (local_fitness[i]<=f_min){
                    f_min=local_fitness[i];
                }
            }
            
            
            
            if (f_min<=T(0.1)){
                return true;
            }
            else{
                std::cout<<"Minimum error in this iteration is :"<<f_min<<"\n";
                return false;
            }
            
        }
        
        
        
        public:
        /*! Creates the class.
            @param[in] _options Configures the Newton Gauss optimizer internally, such as choosing the derivative evaluation type (e.g. Finite Differences) and line search method.
            @param[in] _evaluator Steers how data is loaded and evaluated (e.g. parsed from file, given from within UG4) */
        ParticleSwarmOptimizer(const PSOOptions& _options,E& _evaluator):options(_options), evaluator(_evaluator){
                n_particles=options.get_n_particles();
                n_groups=options.get_n_groups();
                max_iterations=options.get_max_iterations();
                particle_w = options.get_particle_w();
                particle_c = options.get_particle_c();
                if(n_particles <=0 || n_groups <= 0 || options.get_max_iterations() <= 0){
                    throw std::invalid_argument( "Number of Particles or Number of Groups or Max. Iterations is invalid");

                }

        }
        
        /*! Runs the Particle Swarm Optimization algorithm. Only this function has to be called to run the complete procedure.
        @param[in] initial_params Initial parameters containing names and bounds of the parameters
        \return Code indicating success or failure of running the Particle Swarm Optimization procedure.
        */
        template<class T>
        ErrorCode run(EVarManager<T>& estimated_params, const std::vector<std::string>& param_names, const std::vector<T>& bounds){

            size_t n=param_names.size();
            std::vector<Particle<T>> particles;
            
            saved_losses_in_past_iteration = std::vector<double>();
            std::vector<T> target_data;
            std::vector<T> target_times;
            ErrorCode load_code=evaluator.load_target(target_times,target_data); //load target vector

            if (load_code!=ErrorCode::NoError){
                std::cerr<<"Error loading target data!\n";
                return load_code;
            }

            std::vector<T> local_fitness(n_groups,T(std::numeric_limits<double>::max()));
            std::vector<int> best_id(n_groups,-1); //ids of the most successful particle in each group
            std::vector<std::vector<T>> local_best_position(n_groups);
            std::vector<std::vector<int>> groups; //groups with respective group ids
            
            initialize_positions(param_names,bounds,target_times,target_data, particles,groups,local_fitness,local_best_position,best_id);
            

            int iter=0;
            std::vector<int> indices(n_particles);
            
            for (int i=0;i<n_particles;i++){
                indices[i]=particles[i].get_group_id();
            }

            auto rng = std::default_random_engine{};
            std::vector<T> global_minimum_position(n); //best position of all groups
            
            T previous_minimum_fitness=T(std::numeric_limits<double>::max());
            bool redraw_topology=false;
            bool schranke=has_converged(local_fitness);
            
            #if(DEBUG)
                std::cout << "PSO run debug print4 pre while" << std::endl;
            #endif
        
            while (iter<max_iterations && schranke==false){
                std::cout<<"Starting Particle Swarm Optimization iteration "<<iter<<"\n";
                std::vector<EVarManager<T>> evaluations(n_particles);
                
                /*
                std::cout<<"local fitness anfang loop:\n";
                for (int i=0;i<n_groups;i++){
                    std::cout<<local_fitness[i]<<"\n";
                }
                
                std::cout<<"Fitness anfang loop\n";
                for (auto& x: particles){
                    x.print_fitness();
                }
                
                std::cout<<"Positionen anfang loop\n";
                for (auto& x: particles){
                    auto k=x.get_position();
                    for (auto y:k){
                        std::cout<<y<<"  ";
                    }
                    std::cout<<"\n";
                }
                
                std::cin.get();
                */
                //Update the search direction for each particle
                
                //update personal_fitness
                if (redraw_topology==false){
                    
                for (int i=0;i<n_particles;i++){
                    std::vector<T> temp;
                    //std::cout<<"i in redraw_topology false: "<<i<<"\n";
                    int group_id=particles[i].get_group_id();
                //    std::cout<<"vor update direction\n";
                    particles[i].update_direction(target_data,bounds,evaluations[i],param_names,local_best_position[group_id]);
                        
                    
                }
                }
                else{
                    //std::cout<<"!!!Redraw topology!!!\n";
                    redraw_topology=false;
                    for (int i=0;i<n_particles;i++){
                        particles[i].update_direction_random(bounds,evaluations[i],param_names);
                    }
                }
                
                
                /*
                std::cout<<"Positionen nach update_direction\n";
                for (auto& x: particles){
                    auto k=x.get_position();
                    for (auto y:k){
                        std::cout<<y<<"  ";
                    }
                    std::cout<<"\n";
                }
                */
            
                
                //Evaluate the new positions
                ErrorCode e;
            
                std::vector<std::vector<T>> evals=evaluator.eval(evaluations, target_times, e,"Evaluating Particles");
        
                for (int i=0;i<n_particles;i++){
                    particles[i].update_personal_best_fitness(evaluator,target_data,evals[i],local_fitness,local_best_position, best_id);
                }
            
        
                /*
                std::cout<<"local fitness nach update_best_fitness:\n";
                for (int i=0;i<n_groups;i++){
                    std::cout<<local_fitness[i]<<"\n";
                }
                
                
                std::cout<<"local_fitness Positionen nach update_best_fitness\n";
                for (auto& x: local_best_position){
                    for (auto y:x){
                        std::cout<<y<<"  ";
                    }
                    std::cout<<"\n";
                }
                
                std::cin.get();
                */
                /*
                std::cout<<"Fitness before:\n";
                for (auto& x: particles){
                    x.print_fitness();
                }
                */
                
                //Get lowest local fitness
                T minimum_fitness=T(std::numeric_limits<double>::max());        
                for (int i=0;i<n_groups;i++){
                    if (local_fitness[i]<=minimum_fitness){
                        minimum_fitness=local_fitness[i];
                        global_minimum_position=local_best_position[i];
                    }
                }
                
                
                /*
                for (int i=0;i<n_particles;i++){
                    int group_id=particles[i].get_group_id();
                    T p_fitness=particles[i].get_personal_best_fitness();
                //    std::cout<<p_fitness<<" vs. "<<local_fitness[group_id]<<"\n";
                    if (local_fitness[group_id]>=p_fitness){
                        local_fitness[group_id]=p_fitness;
                        local_best_position[group_id]=particles[i].get_personal_best_position();
                        best_id[group_id]=i;
                    }
                    if (p_fitness<=minimum_fitness){
                        minimum_fitness=p_fitness;
                    }
                }
                */
/*
                std::cout<<"local fitness:\n";
                for (int i=0;i<n_groups;i++){
                    std::cout<<local_fitness[i]<<"\n";
                }
                
                std::cout<<"Fitness mitte\n";
                for (auto& x: particles){
                    x.print_fitness();
                }
                
    */
                iter++;
                    
                std::string infos=print_info(particles,param_names,iter,best_id,local_fitness, local_best_position);
                std::string RMSE = "RMSE: " + std::to_string(std::sqrt(me / target_times.size())) + "\n";
                evaluator.send_info(RMSE,"summary_of_estimation");

                evaluator.send_info(infos,"summary_of_estimation");
                std::cout<<infos<<"\n";
                
                if ((minimum_fitness-previous_minimum_fitness).abs()<=T(0.0000001)){
                    //redraw_topology=true;
                    
                    //shuffle indices
                    std::shuffle(std::begin(indices), std::end(indices), rng);
                    //std::random_shuffle(indices.begin(),indices.end()); //old way of doing it
                    for (int i=0;i<n_particles;i++){
                        particles[i].set_group_id(indices[i]);
                    }
                    local_fitness=std::vector<T>(n_groups,T(std::numeric_limits<double>::max()));
                    //redetermine local_best_fitness
                    for (int i=0;i<n_particles;i++){
                    int group_id=particles[i].get_group_id();
                    T p_fitness=particles[i].get_personal_best_fitness();
                //    std::cout<<p_fitness<<" vs. "<<local_fitness[group_id]<<"\n";
                    if (local_fitness[group_id]>=p_fitness){
                        local_fitness[group_id]=p_fitness;
                        local_best_position[group_id]=particles[i].get_personal_best_position();
                        best_id[group_id]=i;
                    }
                }
                    
                }
                saved_losses_in_past_iteration.push_back((double)minimum_fitness);
                previous_minimum_fitness=minimum_fitness;
            
            }
            
            #if(DEBUG)
                std::cout << "PSO run debug print4 post where" << std::endl;
            #endif

            EVarManager<T> parameters;
            
            #if(DEBUG)
                std::cout << "PSO run debug print5 pre for" << std::endl;
            #endif

            for (int i=0; i<n;i++){
                parameters.add(param_names[i],EVar<T>(global_minimum_position[i],bounds[2*i],bounds[(2*i)+1]));
            }

            #if(DEBUG)
                std::cout << "PSO run debug print5 post for" << std::endl;
            #endif

            estimated_params=parameters;
            estimated_params.last_mean_error=me/target_times.size();

            #if(DEBUG)
                std::cout << "PSO run debug print6 pre if" << std::endl;
            #endif

            if (schranke){
                evaluator.send_parameters(parameters, "These are the estimated parameters of the problem.");
            }
            else{
                evaluator.send_parameters(parameters, "These are the estimated parameters of the problem. The run did finish but not converge.");
            }
            std::cout<<"Particle Swarm Optimization finished\n";
            return ErrorCode::NoError;
        }
        
        std::vector<double> saved_losses_in_past_iteration;
            
            std::vector<double> get_saved_losses_in_past_iteration_as_double() const{
                return saved_losses_in_past_iteration;
            }

        
    };
    
}
