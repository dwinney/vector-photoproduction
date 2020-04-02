// Axial-vector meson photoproduction proceeding through a vector meson exchange
//
// Author:       Daniel Winney (2020)
// Affiliation:  Joint Physics Analysis Center (JPAC)
// Email:        dwinney@iu.edu
// ---------------------------------------------------------------------------

#ifndef _AXIAL_
#define _AXIAL_

#include "amplitude.hpp"

class vector_meson_exchange : public helicity_amplitude
{
public:
  // Constructor
  vector_meson_exchange(reaction_kinematics * xkinem, double mass)
  : amplitude(xkinem). mEx2(mass*mass)
  {};

  // Copy constructor
  vector_meson_exchange(const vector_meson_exchange & old)
  : amplitude(old), mEx2(old.mEx2),
    gGamma(old.gGamma), gV(old.gV), gT(old.gT)
  {};

  // Setting utility
  void set_params(std::vector<double> params)
  {
    gGamma = params[0];
    gV = params[1];
    gT = params[2];
  };

  // Assemble the helicity amplitude by contracting the lorentz indices
  std::complex<double> helicity_amplitude(std::vector<int> helicities, double s, double zs);
  
private:
  // Mass of the exchange
  double mEx2;

  // Couplings to the axial-vector/photon and vector/tensor couplings to nucleon
  double gGamma, gV, gT;

  // Four-momentum of the exhange
  std::complex<double> vector_momenta(int mu, double s, double zs);

  // Photon - Axial Vector - Vector vertex
  std::complex<double> top_vertex(int mu, int nu, int lam_gam, int lam_vec, double s, double zs);

  // Nucleon - Nucleon - Vector vertex
  std::complex<double> bottom_vertex(int mu, int nu, int lam_targ, int lam_rec, double s, double zs);

  // Vector propogator
  std::complex<double> vector_propagator(int mu, int nu, double s, double zs);
};

#endif
