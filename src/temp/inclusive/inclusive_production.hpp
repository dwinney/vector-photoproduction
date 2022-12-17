// Abstract class for the invariant cross-section from a triple regge interaction.
// Contains inclusve_kinematics objects as well as dynamical objects with either
// 'JPAC' or 'Field & Fox' models.
//
// Author:       Daniel Winney (2022)
// Affiliation:  Joint Physics Analysis Center (JPAC)
// Email:        dwinney@iu.edu
// ---------------------------------------------------------------------------

#ifndef INCLUSIVE
#define INCLUSIVE

#include "inclusive_kinematics.hpp"

#include "Math/IntegratorMultiDim.h"
#include "Math/GSLIntegrator.h"
#include "Math/IntegrationTypes.h"
#include "Math/Functor.h"

#include <functional>
#include <vector>

namespace jpacPhoto
{
    //--------------------------------------------------------------------
    // Abstract class for which any parameterization will inherit
    // This basically contains the kinematics stuff and integration for cross-sections

    class inclusive_production
    {
        public:
        // Constructor only needs a kinematics object
        inclusive_production(double produced_mass, std::string amp_id = "")
        : _identifier(amp_id)
        {
            _kinematics = new inclusive_kinematics(produced_mass);
        };

        // Deconstructor frees up the kinematics pointer
        ~inclusive_production()
        {
            delete _kinematics;
        };

        inclusive_kinematics * _kinematics;

        // Set whether we should assume the independent variables are (t, M2) or (t, x)
        // See _useTX below
        inline void set_TX(bool opt){ _useTX = opt; };

        // Print out the id
        inline std::string get_id(){ return _identifier; };

        inline void set_debug(int d){ _debug = d; };

        //--------------------------------------------------------------------
        // d3sigma/d3p (invariant cross-section)
        // These need to be specified by a specific parameterization
        // The third argument (double mm) can be either M2 or x
        // which is assumed to correspond to the _useTX member above

        virtual double d3sigma_d3p(double s, double t, double mm) = 0;
        
        // Alias function to rename d3sigma_d3p to more human name
        inline double invariant_xsection(double s, double t, double mm){ return d3sigma_d3p(s, t, mm); };

        // Integrated cross-sections

        // (t, M2)
        double dsigma_dt(double s, double t);     // integrated over M2
        double dsigma_dM2(double s, double M2);   // integrated over t

        // (x, y2)
        double dsigma_dy2(double s, double y2);   // integrated over x
        double dsigma_dx(double s,  double x);    // integrated over pT2

        // Fully integrated
        double integrated_xsection(double s);   

        protected:

        // String identifier just like exclusive amplitudes
        std::string _identifier;

        // Different parameterizations may use different variables which make things tricky
        // when integrating. So I include this flag (set to default as false):
        // False -> assume independent variables are t and M2 
        // True  -> assume independent variables are t and x
        bool _useTX = false;    

        int _debug = 0;
    };
};

#endif 