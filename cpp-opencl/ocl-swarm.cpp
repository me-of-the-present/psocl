/*swarm.cpp
implementation for swarm
for openclless version

Copyright 2018 by Aaron Klapatch,
code derived from http://www.swarmintelligence.org/tutorials.php
along with some help from Dr. Ebeharts presentation at IUPUI.
*/

#include "swarm.hpp"

#include <iostream>

///sets dimensions to 1 and number of particles to 100 and w to 1.0
swarm::swarm(){

    ///set swarm characteristics to defaults
    partnum=DEFAULT_PARTNUM;
    dimnum=DEFAULT_DIM;
    w = DEFAULT_W;
    c1=C1
    c2=C2
    gfitness=-HUGE_VAL;    
	
	///gets up to 3 platforms
	ret = clGetPlatformIDs(PLATFORM_NUM, &platform_id, &ret_num_platforms);
	
	///gets up to 3 devices
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, DEVICE_NUM, &device_id, &ret_num_devices);
	
	///gets a context with up to 3 devices
	context = clCreateContext(NULL, ret_num_devices, &device_id, NULL, NULL, &ret);
	
	///get command queue
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    ///create memory buffer fro gfit and write to it
    gfitbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, gfitbuf, CL_TRUE, 0, sizeof(cl_float), &gfitness, 0, NULL, NULL);

    ///create and write w to memory
    wbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, wbuf, CL_TRUE, 0, sizeof(cl_float), &w, 0, NULL, NULL);

    ///get memory for and write to c1buffer
    c1buf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, c1buf, CL_TRUE, 0, sizeof(cl_float), &c1buf, 0, NULL, NULL);

    ///get memory for and write to c2buffer
    c2buf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, c2buf, CL_TRUE, 0, sizeof(cl_float), &c2buf, 0, NULL, NULL);

    ///creates buffers for every resource
    presentbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*dimnum*sizeof(cl_float), NULL, &ret);

    pbestbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*sizeof(cl_float), NULL, &ret);

    vbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*dimnum*sizeof(cl_float), NULL, &ret);

    pfitnessbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*sizeof(cl_float), NULL, &ret);

    fitnessbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*sizeof(cl_float), NULL, &ret);
}

///sets dimensions to 1 and number of particles to 100 and w to 1.5
swarm::swarm(uint numdims, uint numparts,cl_float inw){

    ///set swarm characteristics to defaults
    partnum=numparts;
    dimnum=numdims;
    w = DEFAULT_W;
    gfitness=-HUGE_VAL;    
  
    ///gets up to 3 platforms
	ret = clGetPlatformIDs(PLATFORM_NUM, &platform_id, &ret_num_platforms);
	
	///gets up to 3 devices
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, DEVICE_NUM, &device_id, &ret_num_devices);
	
	///gets a context with up to 3 devices
	context = clCreateContext(NULL, ret_num_devices, &device_id, NULL, NULL, &ret);
	
	///get command queue
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    ///create memory buffer fro gfit and write to it
    gfitbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, gfitbuf, CL_TRUE, 0, sizeof(cl_float), &gfitness, 0, NULL, NULL);

    ///create and write w to memory
    wbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, wbuf, CL_TRUE, 0, sizeof(cl_float), &w, 0, NULL, NULL);

    ///get memory for and write to c1buffer
    c1buf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, c1buf, CL_TRUE, 0, sizeof(cl_float), &c1buf, 0, NULL, NULL);

    ///get memory for and write to c2buffer
    c2buf=clCreateBuffer(context, CL_MEM_READ_WRITE,sizeof(cl_float), NULL, &ret);
    ret=clEnqueueWriteBuffer(command_queue, c2buf, CL_TRUE, 0, sizeof(cl_float), &c2buf, 0, NULL, NULL);

    ///creates buffers for every resource
    gbestbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,dimnum*sizeof(cl_float), NULL, &ret);

    presentbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*dimnum*sizeof(cl_float), NULL, &ret);

    pbestbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*dimnum*sizeof(cl_float), NULL, &ret);

    vbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*dimnum*sizeof(cl_float), NULL, &ret);

    pfitnessbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*sizeof(cl_float), NULL, &ret);

    fitnessbuf=clCreateBuffer(context, CL_MEM_READ_WRITE,partnum*sizeof(cl_float), NULL, &ret);  
}

///deallocate all memory
swarm::~swarm(){
    
    ///end queue
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
	
    ///get rid of kernel and program
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);

    ///release all used memory
    ret = clReleaseMemObject(fitnessbuf);
    ret = clReleaseMemObject(pfitnessbuf);
    ret = clReleaseMemObject(presentbuf);
    ret = clReleaseMemObject(vbuf);
    ret = clReleaseMemObject(presentbuf);
    ret = clReleaseMemObject(wbuf);
    ret = clReleaseMemObject(c1buf);
    ret = clReleaseMemObject(c2buf);
    ret = clReleaseMemObject(gfitbuf);
    ret = clReleaseMemObject(gbestbuf);

    ///finish up queue and context
    ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
}

///sets number of particles
void swarm::setpartnum(int num){
    
    delete [] pfitnesses;
    delete [] fitnesses;
    
    while(partnum--){
        delete [] presents[partnum];
        delete [] pbests[partnum];
        delete [] v[partnum];
    }

    delete [] pbests;
    delete [] presents;
    delete [] v;

    try {
        ///reset particle swarm #
        partnum=num;

        pfitnesses = new cl_float [num];
        fitnesses= new double [num];
        
        pbests= new cl_float * [num];
        presents= new cl_float * [num];    
        v= new cl_float * [num];

        ///set all vectors to proper dimensions
        while(num--){
            presents[num]= new cl_float [dimnum];
            pbests[num]= new cl_float [dimnum];
            v[num]= new cl_float [dimnum];
            pfitnesses[num]=-HUGE_VAL;
        }
    } catch (std::bad_alloc& ac) {
        std::cerr << "Memory allocation failed: "<<ac.what() <<std::endl;
        exit(1);
    }    
}

///sets number of dimensions
void swarm::setdimnum(int num){
    
    dimnum=num;
    
    try {    

        delete [] gbest;
        gbest = new cl_float [num];
        
        int i;
        for(i=0;i<partnum;++i){
            delete [] presents[i];
            delete [] pbests[i];
            delete [] v[i];

            presents[i] = new cl_float [num];
            pbests[i] = new cl_float [num];
            v[i] = new cl_float [num];
        }  
    } catch (std::bad_alloc& ac) {
        std::cerr << "Memory allocation failed: "<<ac.what() <<std::endl;
        exit(1);
    } 
}

///set inertial weight
void swarm::setweight(float nw){
    w=nw;
}

///set behavioral constants
void swarm::setconstants(float nc1,float nc2){
    c1=nc1;
    c2=nc2;
}

///distribute particle linearly from lower bound to upper bound
void swarm::distribute(cl_float * lower, cl_float * upper){
    
    ///store bounds for later
    upperbound=upper;
    lowerbound=lower;
    
    int i,j;
    
    try{
        double * delta = new double [dimnum];
        
        for(i=0; i<dimnum; ++i){
            delta[i]=(upperbound[i] - lowerbound[i])/(partnum-1);
            gbest[i]=0;
            
            for(j=0;j<partnum;++j){
                presents[j][i]=j*delta[i] + lowerbound[i];
                pbests[j][i]=0;
                v[j][i]=0;
            }
        }

        delete [] delta;

    } catch (std::bad_alloc& ac) {
        std::cerr << "Memory allocation failed: "<<ac.what() <<std::endl;
        exit(1);
    } 
}

///run the position and velocity update equation
void swarm::update(cl_int times, cl_float (*fitness) (cl_float*)){
        
    int i,j;

    ///make random number generator C++11
    std::random_device gen;
    std:: uniform_real_distribution<double> distr(1,0);

    while(times--){
        for(i=0;i<partnum;++i){
            for(j=0;j<dimnum;++j){

                ///update velocity                
                v[i][j]=w*v[i][j] + c1*distr(gen)*(pbests[i][j]-presents[i][j]) +c2*distr(gen)*(gbest[j]-presents[i][j]);

                ///update position
                presents[i][j]=presents[i][j]+v[i][j];

                ///if it exceeds the bounds
                if(presents[i][j]>upperbound[j]){
                    presents[i][j]=upperbound[j];

                ///if it goes below the lower bound
                } else if (presents[i][j]< lowerbound[j]){
                    presents[i][j]=lowerbound[j];
                }
            }

            ///get fitness
            fitnesses[i] = fitness(presents[i]);

            ///if the fitness is better than the particle best store the best position
            if(fitnesses[i]>pfitnesses[i]){
                
                ///set new fitness
                pfitnesses[i]=fitnesses[i];
                
                ///store position
                for(j=0;j<dimnum;++j){
                    pbests[i][j]=presents[i][j];
                }

                ///if fitness is better than global fitness
                if(fitnesses[i]>gfitness){
                    
                    ///set new fitness
                    gfitness=fitnesses[i];

                    ///store best position
                    for(j=0;j<dimnum;++j){
                        gbest[j]=presents[i][j];
                    }
                }
            }
        }
    }
}

///returns best position of the swarm
cl_float * swarm::getgbest(){
    return gbest;
}   

///returns the fitness of the best particle
cl_float swarm::getgfitness(){
    return gfitness;
}
