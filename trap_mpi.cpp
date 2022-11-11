//Trapezoid rule with MPI
#include<iostream>
#include<mpi.h>
using namespace std;
using namespace MPI;
float evaluateFunction(float b);
float evaluateTrapMeth(float start_point, float end_point, int local_traps, float height);


int main(int argc, char *argv[])
{
    float a = 0.0;
    float b = 1.0;
    int n = 90; 
    float h, total = 0.0;
    h = (b-a)/n; 
    int tag = 0 ;

    Init();
    int num_procs = COMM_WORLD.Get_size();
    int local_traps = 0;
    local_traps = n/(num_procs-1);

    if (COMM_WORLD.Get_rank()!=0) // Non-zero ranked processes compute areas according to the specfied partition
    {   int local_rank = COMM_WORLD.Get_rank();
        float integral_send, start_point, end_point  = 0.0;
        start_point = a + (local_rank-1)*local_traps*h;
        end_point = a + (local_rank)*local_traps*h;
        integral_send = evaluateTrapMeth(start_point, end_point, local_traps, h);
        COMM_WORLD.Send(&integral_send, 1, FLOAT, 0, tag);
    }

    else // Process with rank 0 adds areas computed by other processes
    {   
    for (int i=1; i<=num_procs-1; i++)
    {
        float integral_recv = 0.0;
        COMM_WORLD.Recv(&integral_recv, 1, FLOAT, i, tag);
        total = total + integral_recv;
    }
    }

    if (COMM_WORLD.Get_rank()==0)
    {   
        cout<<"The integral is "<< total<<"\n";
    }
    Finalize();
    
    return 0;
}


float evaluateFunction(float b)
{
    return 2*b*b + 2;
}

float evaluateTrapMeth(float start_point, float end_point, int local_traps, float height)
{
    float local_integral = 0.0;
    local_integral = 0.5*(evaluateFunction(start_point) + evaluateFunction(end_point));

    for (int i=1; i<=local_traps-1; i++)
    {
        local_integral = local_integral + evaluateFunction(start_point + i*height);
    }
    local_integral = local_integral*height;
    return local_integral;
}
