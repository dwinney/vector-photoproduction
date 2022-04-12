// Form of the triple Regge interaction using JPAC's parameterization
// i.e. using the t dependence from properly normalized Regge propagators
// and M2 dependence from the total hadronic cross-section of the bottom vertex.
//
// Author:       Daniel Winney (2022)
// Affiliation:  Joint Physics Analysis Center (JPAC)
// Email:        dwinney@iu.edu
// ---------------------------------------------------------------------------

#include "inclusive/triple_regge.hpp"

// ---------------------------------------------------------------------------
// Parse the passed amplitude_name string from an exclusive amplitude
// to make sure the appropriate coupling function is used for the top vertex
void jpacPhoto::triple_regge::initialize(std::string amp_name)
{
    // Pion exchange on top vertices
    if (amp_name == "pseudoscalar_exchange")
    {
        // Axial-vector - photon - psuedoscalar coupling
        _coupling = [&](double t)
        {
            return  (_g / _kinematics->_mX) * (t - _kinematics->_mX2);
        };

        // Default: pi- exchange with the PDG parameterization (no resonances)
        _sigma_tot = new PDG_parameterization(M_PION, M_PROTON, {-1., 1., 9.56, 1.767, 18.75});
    }
    else 
    {
        _coupling = [&](double x)
        {
            return 0.;
        };

        _sigma_tot = new zero_xsection();
    };
};

// ---------------------------------------------------------------------------
// Change the default sigma_total from initialize to a user-selected one
void jpacPhoto::triple_regge::set_sigma_total(sigma_option opt)
{
    // We need to make sure to free up _sigma_tot first
    delete _sigma_tot;

    switch(opt)
    {
        case PDG_pipp_onlyRegge:
        {
            _sigma_tot = new PDG_parameterization(M_PION, M_PROTON, {+1., 1., 9.56, 1.767, 18.75});
            break;
        } 
        case PDG_pimp_onlyRegge:
        {
            _sigma_tot = new PDG_parameterization(M_PION, M_PROTON, {-1., 1., 9.56, 1.767, 18.75});
            break;
        }
        case PDG_pipp_withResonances:
        {
            _sigma_tot = new PDG_parameterization(M_PION, M_PROTON, {+1., 1., 9.56, 1.767, 18.75}, "rpp2020-pipp_total.dat");
            break;
        } 
        case PDG_pimp_withResonances:
        {
            _sigma_tot = new PDG_parameterization(M_PION, M_PROTON, {-1., 1., 9.56, 1.767, 18.75}, "rpp2020-pimp_total.dat");
            break;
        }
        case JPAC_pipp_onlyRegge:
        {
            _sigma_tot = new JPAC_parameterization(+1, false);
            break;
        }
        case JPAC_pimp_onlyRegge:
        {
            _sigma_tot = new JPAC_parameterization(-1, false);
            break;
        }
        case JPAC_pipp_withResonances:
        {
            _sigma_tot = new JPAC_parameterization(+1, true);
            break;
        }
        case JPAC_pimp_withResonances:
        {
            _sigma_tot = new JPAC_parameterization(-1, true);
            break;
        }
        default:
        {
            _sigma_tot = new zero_xsection();
        };
    };

};

// ---------------------------------------------------------------------------
// Evaluate the invariant amplitude
double jpacPhoto::triple_regge::d3sigma_d3p(double s, double t, double mm)
{
    // Make sure to pass the CM energy to the kinematics
    _kinematics->_s = s;

    // Things tend to blow up at exactly x = 1
    if (_useTX && (abs(mm - 1) < 0.001)) return 0.;

    // Coupling squared
    double coupling2   = _coupling(t) * _coupling(t);

    // Form factor with tprime corresponding to the exclusive limit
    double formfactor2 = exp(2. * _b * (t - _kinematics->TMINfromM2( _kinematics->_minM2 )));

    // phase_space factor (depends on whether mm is x or M2)
    double s_piece;
    (_useTX) ? (s_piece = (1. - mm)) : (s_piece = mm / s);

    double exchange_propagator2;
    if (_useRegge)
    {
        double alpha      = std::real(_trajectory->eval(t));
        double alphaPrime = std::real(_trajectory->slope());

        // First check t isnt too big to make the gamma function blow up
        if ( _b + alphaPrime - alphaPrime * log(- alphaPrime * t) < 0.) return 0.;

        std::complex<double> signature_factor = (1. + double(_trajectory->_signature) * exp(- XI * M_PI * alpha)) / 2.; 
        double t_piece = std::norm(alphaPrime * signature_factor * cgamma(double(_trajectory->_minJ) - alpha));

        exchange_propagator2 = t_piece * pow(s_piece, -2. * alpha);
    }
    else
    {
        double pole          = 1. / (_exchange_mass2 - t);                                     // Simple pole 
        exchange_propagator2 = pole * pole * pow(s_piece, -2. * double(_trajectory->_minJ));   // Squared
    };

    double sigma_tot;
    (_useTX) ? (sigma_tot = _sigma_tot->eval(s * (1. - mm))) : (sigma_tot = _sigma_tot->eval(mm));

    return sigma_tot * coupling2 * formfactor2 * exchange_propagator2 * s_piece / pow(4.* M_PI, 3.);
};  