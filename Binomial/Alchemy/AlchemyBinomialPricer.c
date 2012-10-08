#include <stdlib.h>
#include <stdio.h>
#include <math.h>
 
//Header file for AS3 interop APIs
#include "AS3.h"
 
static AS3_Val getPrice(void* self, AS3_Val args)
{
    double N, T, S, K, v, r, price;
    AS3_ArrayValue( args, "DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, DoubleType", &N, &T, &S, &K, &v, &r);
 
    double dt = T/N; //one time step
 
    double u = 1 + v*sqrt(dt); //up-tick
    double d = 1 - v*sqrt(dt); //down-tick
    double p = 0.5 + r*sqrt(dt)/(2*v); //risk-neutral probability of up-tick
    double df = 1/(1+r*dt); //discount factor over 1 time step, dt
 
    double* optionValues = malloc((N+1) * sizeof(double));
 
    //populate the tree (for N-steps there will be N+1 values)
    int i, j;
    double ST;
    for (i=0; i < N+1; i++) 
    {
        ST = S * pow(u, i) * pow(d, N-i);
        optionValues[i] = (ST > K)? ST - K : 0;
    }
 
    //now work backwards to get expected 
    //option value at each previous stage
    for (i=N; i >= 0; i--)
    {
        for(j=0; j<i; j++)
        {
            optionValues[j] = (p*optionValues[j+1] + (1-p)*optionValues[j])*df;
        }
    }
 
    price = optionValues[0];
    free(optionValues);
 
    return AS3_Number(price);
}
 
int main()
{
    //define the methods exposed to ActionScript
    //typed as an ActionScript Function instance
    AS3_Val getPriceMethod = AS3_Function( NULL, getPrice );
 
    // construct an object that holds references to the functions
    AS3_Val result = AS3_Object( "getPrice: AS3ValType", getPriceMethod );
 
    // Release
    AS3_Release( getPriceMethod );
 
    // notify that we initialized -- THIS DOES NOT RETURN!
    AS3_LibInit( result );
 
    // should never get here!
    return 0;
}