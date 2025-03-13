#include "support/configCosmos.h"
#include "physics/controllib.h"

//! \addtogroup controllib_functions
//! @{

//! Calculate control torque - angular attitude and rate based
/*! Calculate the control torque that should be applied to reach a target attitude from a current attitude.
    \param gain Portion of change to be achieved per time step.
    \param tatt Target ICRF attitude quaternion.
    \param catt Current ICRF attitude quaternion.
    \moi Principle moments of inertia.
*/
rvector calc_control_torque(double lag, Convert::qatt tatt, Convert::qatt catt, rvector moi)
{
    rmatrix mom, rm;
    double dalp, domg, dt;
    rvector alpha, omega, torque;
    rvector distance;
    quaternion dsq2;

    if (lag > 0.)
    {
        // rvector mtorque  = -1. * tv.cross(Vector(rv_mmult(mom, tv.to_rv())) + physics.hmomentum);

        // Calculate adjustment for ICRF distance

        if (length_q(tatt.s))
        {
            dsq2 = q_mult(tatt.s,q_conjugate(catt.s));
            distance = rv_quaternion2axis(dsq2);
            domg = length_rv(distance);
            if (domg > DPI)
            {
                dsq2 = q_smult(-1.,dsq2);
                distance = rv_quaternion2axis(dsq2);
            }

            omega = rv_smult(1./lag,distance);
        }
        // If Target quaternion is zero, adjust only for velocity
        else
        {
            omega = rv_zero();
        }

        // Match velocity
        omega = rv_add(omega,tatt.v);
        alpha = rv_sub(omega,catt.v);
        dalp = length_rv(alpha) / lag;
        alpha = rv_smult(dalp,rv_normal(alpha));

        // Conversion from ICRF to Body
        rm = rm_quaternion2dcm(catt.s);
        // Moment of Inertia in Body
        mom = rm_diag(moi);
        // Moment of Inertia in ICRF
        mom = rm_mmult(rm_transpose(rm),rm_mmult(mom,rm));
        torque = rv_mmult(mom,alpha);

        return (torque);
    }
    else
    {
        return (rv_zero());
    }
}


//! @}
