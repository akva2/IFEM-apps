#include <iostream>
#include <fstream>
#include <LRSpline/LRSplineSurface.h>

using namespace std;
using namespace LR;

int main(int argc, char **argv) {
  // setup problem parameters
  int p1 = 3;   // polynomial order (degree + 1)
  int p2 = 3; 
  int n1 = 6;  // number of basisfunctions 
  int n2 = 6;  // remember that n=p gives a single element (knot span)
                // i.e. when changing p, also change n
  double lx = 0.001; // Length of domain
  double ly = 0.001;
      
  const int lr_steps = 0; // Nr of refinement steps
        
  // construct object
  LRSplineSurface lr = LRSplineSurface(n1,n2, p1,p2);

  for(Basisfunction* b : lr.getAllBasisfunctions()) {
    b->cp()[0] *= lx; // scale x-coordinate of this controlpoint
    b->cp()[1] *= ly; // scale y-coordinate of this controlpoint
  }

  if (lr_steps > 0) {
      
    //lr.setRefStrat(LR_MINSPAN);
    //lr.setMaxAspectRatio(2.0);

    double delta = 0.05;
      
    for (int ref=0; ref<lr_steps; ++ref) {

      cout << "Refinement cycle #" << ref+1 << endl;

      std::vector<int> idx;

      int i = 0;
      for(Basisfunction* b : lr.getAllBasisfunctions()) {
	// Find center of basis support
	double cx = (b->getParmin(0) + b->getParmax(0))/2.0;
	double cy = (b->getParmin(1) + b->getParmax(1))/2.0;

	// Refine if center is close to center
	if ( (cx-0.5)*(cx-0.5) + (cy-0.5)*(cy-0.5) < delta*delta ) {
	  idx.push_back(i);
	  cout << "  Tag basis function " << i << " for refinement" << endl;
	}

	++i;
      }

      lr.refineBasisFunction(idx);

      delta = 2.0*delta/4.0;
	
    }

  }

  // make the C^{-1} crack
  lr.insert_const_v_edge(0.5, // constant v-value
			 0,   // u_start
			 0.5, // u_end
			 p2); // multiplicity m (continuity is given by p-m-1)

  // write results to screen
  // cout << lr << endl;

  // write results to file
  ofstream myfile;
  myfile.open("unit_slit.lr", std::ofstream::trunc);
  myfile << lr << endl;
  myfile.close();
} 
