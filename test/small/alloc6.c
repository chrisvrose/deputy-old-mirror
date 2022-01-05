#include "harness.h"

int to_alloc;
int ** COUNT(to_alloc) to_nodes;
double * COUNT(to_alloc) coeffs;


int global;

int main() {

  int degree = 8;
  
  to_alloc = degree;
  to_nodes = (int **) malloc(degree*(sizeof(int *)));
  coeffs = (double *) malloc(degree*sizeof(double));

  coeffs[7] = 2.0;
  to_nodes[7] = & global;
  
  return 0;
}
