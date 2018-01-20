#ifndef PSOCL_H
#define PSOCL_H

#include <CL/cl.h>

//struct for each particle 
typedef struct particle {
    int dimensionnum;
    double *present, *pbest, fitness;
} particle;

//struct for the swarm
typedef struct swarm {
    int partnum, dimnum;
    double *gbest;
    particle *herd;
} swarm;

//struct for the opencl accelerated swarm
typedef struct clswarm {
    int partnum, dimnum;
    double *gbest;
    particle *herd;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue command_queue;
} clswarm;

//swarm initializaion
void initswarm(char type, int dimensionnum, int partnum, swarm school);

void clinitswarm(char type, int dimensionnum, int partnum, clswarm school);

//swarm particle distribution
void distributeparticles(swarm school);

void cldistributeparticles(clswarm school);

//running the swarm
void clrunswarm(int iterations, clswarm school, double (*fitness)(particle *);

void runswarm(int iterations, swarm school,double (*fitness)(particle *));

//releasing the swarm
void releaseswarm(swarm school);

void clreleaseswarm(clswarm school);




















#endif