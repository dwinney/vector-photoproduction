// ---------------------------------------------------------------------------
// Comparing the photoproduction cross-sections of the Jpsi 1s and 2s states
// near threshold at GlueX
//
// Author:       Daniel Winney (2020)
// Affiliation:  Joint Physics Analysis Center (JPAC)
// Email:        dwinney@iu.edu
// ---------------------------------------------------------------------------
// References:
// [1] 10.1103/PhysRevD.94.034002
// [2] 10.1103/PhysRevD.100.034019
// ---------------------------------------------------------------------------
// COMMAND LINE OPTIONS:
// -c double          # Change CM angle in degree (default: 0)
// -n int             # Number of points to plot (default: 100)
// -m double          # Maximum energy to plot (default: 5.55 GeV)
// -ratio             # Plot ratio of 2S / 1S dxs (default: false)
// -y "[y1:y2]"       # Custom y bounds in output plot (default: ROOT's auto range)
// -lab               # Display E_lab in the x-axis (default: false)
// ---------------------------------------------------------------------------

#include "constants.hpp"
#include "reaction_kinematics.hpp"
#include "amplitudes/pomeron_exchange.hpp"
#include "regge_trajectory.hpp"

#include "jpacGraph1D.hpp"

#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace jpacPhoto;

int main( int argc, char** argv )
{
  // ---------------------------------------------------------------------------
  // COMMAND LINE OPTIONS
  // ---------------------------------------------------------------------------
  double theta = 0.;
  double max = 25;
  int N = 25; // how many points to plot
  std::string ylabel = "#sigma (#gamma N #rightarrow Y N)   [nb]"; bool INTEG = true;
  double y[2]; bool custom_y = false;
  std::string xlabel = "W   [GeV]"; bool LAB = false;
  std::string filename = "Y4220_photoproduction.pdf";
  for (int i = 0; i < argc; i++)
  {
    if (std::strcmp(argv[i],"-c")==0) theta = atof(argv[i+1]);
    if (std::strcmp(argv[i],"-n")==0) N = atoi(argv[i+1]);
    if (std::strcmp(argv[i],"-m")==0) max = atof(argv[i+1]);
    if (std::strcmp(argv[i],"-f")==0) filename = argv[i+1];
    if (std::strcmp(argv[i],"-y")==0) {y_range(argv[i+1], y); custom_y = true;}
    if (std::strcmp(argv[i],"-diff")==0)
    {
      INTEG = false;
      ylabel = "d#sigma/dt  [nb GeV^{-2}]";
    }
    if (std::strcmp(argv[i],"-lab")==0)
    {
      LAB = true;
      xlabel = "E_{#gamma}   [GeV]";
    }
  }

  // ---------------------------------------------------------------------------
  // Pomeron trajectory
  // ---------------------------------------------------------------------------
  // Set up pomeron trajectory
  // Here we use (real) linear trajectory with intercept and slope only free param

  // Best fit values from [1] from high energy
  linear_trajectory alpha2016(+1, 1.1, 0.11, "pomeron");

  // Best fit values from [2] from near threshold data
  linear_trajectory alpha2019(+1, .94, 0.36, "pomeron");

  // ---------------------------------------------------------------------------
  // Y(4260)
  // ---------------------------------------------------------------------------
  // Set up kinematics, determined entirely by vector meson mass
  reaction_kinematics * kY = new reaction_kinematics(4.220, "Y(4220)");

  pomeron_exchange Y(kY, &alpha2019, false, "Y(4220) (2016 fit)");
  Y.set_params({2.35, .12});

  pomeron_exchange Y2(kY, &alpha2016, true, "Y(4220) (2019 fit)");
  Y2.set_params({2.35, 1.0});


  // ---------------------------------------------------------------------------
  // You shouldnt need to change anything below this line
  // ---------------------------------------------------------------------------

  // Amplitudes to plot
  std::vector<amplitude*> amps;
  amps.push_back(&Y);
  amps.push_back(&Y2);

  // Initialize objects to plot
  jpacGraph1D* plotter = new jpacGraph1D();

  // ---------------------------------------------------------------------------
  // Print the desired observable for each amplitude
  for (int n = 0; n < amps.size(); n++)
  {
    double low;
    (LAB == true) ? (low = E_lab(amps[n]->kinematics->Wth) + EPS)
                  : (low = amps[n]->kinematics->Wth + EPS);

    auto F = [&](double x)
    {
      double s;
      (LAB == false) ? (s = x*x) : (s = W_cm(x) * W_cm(x));

      if (INTEG == false)
      {
        double t = amps[n]->kinematics->t_man(s, theta * deg2rad);
        return amps[n]->differential_xsection(s, t);
      }
      else
      {
        return amps[n]->integrated_xsection(s);
      }
    };

    std::array<std::vector<double>, 2> x_fx = vec_fill(N, F, low, max, true);
    plotter->AddEntry(x_fx[0], x_fx[1], amps[n]->identifier);
  }

  // ---------------------------------------------------------------------------
  // Plotting Settings
  // ---------------------------------------------------------------------------

  // Tweak the axes
  (custom_y == true) ? (plotter->SetYaxis(ylabel, y[0], y[1])) : (plotter->SetYaxis(ylabel));

  double low;
  (LAB == true) ? (low = E_lab(kY->Wth) + EPS)
                : (low = kY->Wth + EPS);
  plotter->SetXaxis(xlabel, low, max);

  plotter->SetLegend(0.6, 0.7);
  plotter->Plot(filename);

  return 1.;
};
