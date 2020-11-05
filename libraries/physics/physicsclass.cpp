#include "physicsclass.h"

namespace Cosmos
{
    namespace Physics
    {
        State::State()
        {
        }

        int32_t State::Init(Propagator *posprop, Propagator *attprop, Propagator *thermprop, Propagator *elecprop)
        {
            int32_t iretn = 0;


            position = posprop;
            attitude = attprop;
            thermal = thermprop;
            electrical = elecprop;

            phys = position->phys;
            loc = position->loc;

            return iretn;
        }

        int32_t State::Increment(double nextutc)
        {
            int32_t count = 0;
            do
            {
                loc->utc += phys->dtj;
                switch (position->type)
                {
                case Propagator::Type::PositionIterative:
                    static_cast<IterativePositionPropagator *>(position)->Propagate();
                    break;
                case Propagator::Type::PositionInertial:
                    static_cast<InertialPositionPropagator *>(position)->Propagate();
                    break;
                case Propagator::Type::PositionGaussJackson:
                    static_cast<GaussJacksonPositionPropagator *>(position)->Propagate();
                    break;
                default:
                    break;
                }
                switch (attitude->type)
                {
                case Propagator::Type::AttitudeIterative:
                    static_cast<IterativeAttitudePropagator *>(attitude)->Propagate();
                    break;
                case Propagator::Type::AttitudeInertial:
                    static_cast<InertialAttitudePropagator *>(attitude)->Propagate();
                    break;
                case Propagator::Type::AttitudeLVLH:
                    static_cast<LVLHAttitudePropagator *>(attitude)->Propagate();
                    break;
                default:
                    break;
                }
                static_cast<ThermalPropagator *>(thermal)->Propagate();
                static_cast<ElectricalPropagator *>(electrical)->Propagate();
                ++count;
            } while (loc->utc < nextutc);
            return count;
        }


        int32_t InertialAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t InertialAttitudePropagator::Propagate()
        {

            return 0;
        }

        int32_t IterativeAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t IterativeAttitudePropagator::Propagate()
        {
            quaternion q1;

            q1 = q_axis2quaternion_rv(rv_smult(phys->dt, loc->att.icrf.v));
            loc->att.icrf.s = q_fmult(q1, loc->att.icrf.s);
            normalize_q(&loc->att.icrf.s);
            // Calculate new v from da
            loc->att.icrf.v = rv_add(loc->att.icrf.v, rv_smult(phys->dt, loc->att.icrf.a));
            loc->att.icrf.utc = loc->utc;
            att_icrf(loc);

            return 0;
        }

        int32_t LVLHAttitudePropagator::Init()
        {
            loc->att.lvlh.utc = loc->utc;
            loc->att.lvlh.s = q_eye();
            loc->att.lvlh.v = rv_zero();
            loc->att.lvlh.a = rv_zero();
            ++loc->att.lvlh.pass;
            att_lvlh(loc);

            return  0;
        }

        int32_t LVLHAttitudePropagator::Propagate()
        {
            loc->att.lvlh.utc = loc->utc;
            loc->att.lvlh.s = q_eye();
            loc->att.lvlh.v = rv_zero();
            loc->att.lvlh.a = rv_zero();
            ++loc->att.lvlh.pass;
            att_lvlh(loc);

            return 0;
        }

        int32_t ThermalPropagator::Init(float temp)
        {
            temperature = temp;
            return 0;
        }

        int32_t ThermalPropagator::Propagate()
        {
            int32_t iretn = 0.;
            float sdheat = 0.;
            float dheat;
            float energyd;

            phys->heat = 0.;
            for (uint16_t i=0; i<phys->triangles.size(); ++i)
            {
                if (phys->triangles[i].external)
                {
                    // Absorption
                    if (phys->triangles[i].irradiation > 0.)
                    {
                        energyd =  phys->dt * phys->triangles[i].irradiation;
                        phys->triangles[i].heat += phys->triangles[i].area * phys->triangles[i].abs * energyd;
                        if (phys->triangles[i].pcell > 0.f)
                        {
                            float efficiency = phys->triangles[i].ecellbase + phys->triangles[i].ecellslope * phys->triangles[i].temp;
                            float power = phys->triangles[i].pcell * phys->triangles[i].area * efficiency * phys->triangles[i].irradiation;
                            phys->triangles[i].heat -= power * phys->dt;
                        }
                    }

                    // Radiation
                    energyd = phys->dt * SIGMA * pow(phys->triangles[i].temp, 4.);
                    dheat = phys->triangles[i].emi * phys->triangles[i].area * energyd;
                    phys->triangles[i].heat -= dheat;
                }

                // Conduction
                //                dheat = phys->dt * .5 * phys->triangles[i].perimeter * (phys->temp - phys->triangles[i].temp) / phys->triangles[i].com.norm();
                //                phys->triangles[i].heat += dheat;
                //                sdheat -= dheat;
            }

            // Compensate for conduction
            for (uint16_t i=0; i<phys->triangles.size(); ++i)
            {
                phys->triangles[i].heat += phys->triangles[i].area * sdheat / phys->area;
                phys->triangles[i].temp = phys->triangles[i].heat / (phys->triangles[i].mass * phys->triangles[i].hcap);
                phys->heat += phys->triangles[i].heat;
            }
            phys->temp = phys->heat / (phys->mass * phys->hcap);
            return iretn;
        }

        int32_t ElectricalPropagator::Init(float bp)
        {
            battery_charge = bp;
            return 0;
        }

        int32_t ElectricalPropagator::Propagate()
        {
            return 0;
        }

        int32_t InertialPositionPropagator::Init()
        {
            return 0;
        }

        int32_t InertialPositionPropagator::Propagate()
        {
            return 0;
        }

        int32_t IterativePositionPropagator::Init()
        {
            return 0;
        }

        int32_t IterativePositionPropagator::Propagate()
        {
            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Setup(uint16_t iorder)
        {
            step.resize(order+2);
            binom.resize(order+2);
            beta.resize(order+2);
            alpha.resize(order+2);
            for (uint16_t i=0; i<order+2; ++i)
            {
                binom[i].resize(order+2);
                beta[i].resize(order+1);
                alpha[i].resize(order+1);
            }
            c.resize(order+3);
            gam.resize(order+2);
            q.resize(order+3);
            lam.resize(order+3);
            order = 0;

            dtsq = phys->dt * phys->dt;

            order2 = iorder/2;
            order = order2 * 2;

            for (uint16_t m=0; m<order+2; m++)
            {
                for (uint16_t i=0; i<order+2; i++)
                {
                    if (m > i)
                        binom[m][i] = 0;
                    else
                    {
                        if (m == i)
                            binom[m][i] = 1;
                        else
                        {
                            if (m == 0)
                                binom[m][i] = 1;
                            else
                            {
                                binom[m][i] = binom[m-1][i-1] + binom[m][i-1];
                            }
                        }
                    }
                }
            }

            c[0] = 1.;
            for (uint16_t n=1; n<order+3; n++)
            {
                c[n] = 0.;
                for (uint16_t i=0; i<=n-1; i++)
                {
                    c[n] -= c[i] / (n+1-i);
                }
            }

            gam[0] = c[0];
            for (uint16_t i=1; i<order+2; i++)
            {
                gam[i] = gam[i-1] + c[i];
            }

            for (uint16_t i=0; i<order+1; i++)
            {
                beta[order+1][i] = gam[i+1];
                beta[order][i] = c[i+1];
                for (uint32_t j=order-1; j<order; --j)
                {
                    if (!i)
                        beta[j][i] = beta[j+1][i];
                    else
                        beta[j][i] = beta[j+1][i] - beta[j+1][i-1];
                }
            }

            q[0] = 1.;
            for (uint16_t i=1; i<order+3; i++)
            {
                q[i] = 0.;
                for (uint32_t k=0; k<=i; k++)
                {
                    q[i] += c[k]*c[i-k];
                }
            }

            lam[0] = q[0];
            for (uint16_t i=1; i<order+3; i++)
            {
                lam[i] = lam[i-1] + q[i];
            }

            for (uint16_t i=0; i<order+1; i++)
            {
                alpha[order+1][i] = lam[i+2];
                alpha[order][i] = q[i+2];
                for (uint32_t j=order-1; j<order; --j)
                {
                    if (!i)
                        alpha[j][i] = alpha[j+1][i];
                    else
                        alpha[j][i] = alpha[j+1][i] - alpha[j+1][i-1];
                }
            }

            for (uint16_t j=0; j<order+2; j++)
            {
                step[j].a.resize(order+1);
                step[j].b.resize(order+1);
                for (uint16_t m=0; m<order+1; m++)
                {
                    step[j].a[order-m] = 0.;
                    step[j].b[order-m] = 0.;
                    for (uint32_t i=m; i<=order; i++)
                    {
                        step[j].a[order-m] += alpha[j][i] * binom[m][i];
                        step[j].b[order-m] += beta[j][i] * binom[m][i];
                    }
                    step[j].a[order-m] *= pow(-1.,m);
                    step[j].b[order-m] *= pow(-1.,m);
                    if (order-m == j)
                        step[j].b[order-m] += .5;
                }
            }
            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Init(vector<tlestruc>lines)
        {
            int32_t iretn = 0;

            loc_clear(step[order+1].loc);
            lines2eci(loc->utc, lines, loc->pos.eci);
            ++loc->pos.eci.pass;
            pos_eci(loc);
            PosAccel(*loc, *phys);
            AttAccel(*loc, *phys);
            step[order2].loc = *loc;

            // Position at t0-dt
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= phys->dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i+1].loc.att.lvlh;
                att_lvlh2icrf(step[i].loc);

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i].loc = step[i-1].loc;

                step[i].loc.utc += phys->dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i-1].loc.att.lvlh;
                att_lvlh2icrf(step[i].loc);

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }

            iretn = Converge();
            phys->utc = loc->utc;

            return iretn;
        }

        //! Initialize Gauss-Jackson orbit using ECI state vector
        /*! Initializes Gauss-Jackson structures using supplied initial state vector.
            \param gjh Reference to ::gj_handle for Gauss-Jackson integration.
            \param order the order at which the integration will be performed (must be even)
            \param mode Mode of physics propagation. Zero is free propagation.
            \param dt Step size in seconds
            \param utc Initial step time as UTC in Modified Julian Days
            \param ipos Initial ECI Position
            \param iatt Initial ICRF Attitude
            \param physics Reference to ::physicsstruc to use.
            \param loc Reference to ::locstruc to use.
        */
        // TODO: split the orbit from the attitude propagation sections of the code
        int32_t GaussJacksonPositionPropagator::Init()
        {
            kepstruc kep;
            double dea;
            uint32_t i;
            quaternion q1;
            int32_t iretn = 0;

            // Make sure ::locstruc is internally self consistent
            ++loc->pos.eci.pass;
            pos_eci(loc);

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);

            // Calculate initial accelerations
            PosAccel(*loc, *phys);
            AttAccel(*loc, *phys);

            // Set central bin to initial state vector
            step[order2].loc = *loc;

            // Position at t0-dt
            eci2kep(loc->pos.eci, kep);

            // Initialize past bins
            for (i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= phys->dtj;
                kep.utc = step[i].loc.utc;
                kep.ma -= phys->dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(-phys->dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(-phys->dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                pos_eci(step[i].loc);

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }

            eci2kep(loc->pos.eci, kep);
            for (i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += phys->dtj;
                kep.utc = step[i].loc.utc;
                kep.ma += phys->dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(phys->dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(phys->dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                pos_eci(step[i].loc);

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }
            iretn = Converge();
            phys->utc = loc->utc;
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Init(vector<locstruc> locs)
        {
            int32_t iretn = 0;

            // Make sure ::locstruc is internally self consistent
            ++loc->pos.eci.pass;
            pos_eci(loc);

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);

            // Calculate initial accelerations
            PosAccel(*loc, *phys);
            AttAccel(*loc, *phys);

            // Set central bin to initial state vector
            step[order2].loc = *loc;

            // Initialize past bins
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= phys->dtj;

                size_t index = locs.size();
                double dutc = 50000.;
                for (uint16_t j=0; j<locs.size(); ++j)
                {
                    if (fabs(locs[j].utc - step[j].loc.utc) > dutc)
                    {
                        dutc = fabs(locs[j].utc - step[j].loc.utc);
                        index = j;
                    }
                }
                if (index < locs.size())
                {
                    step[i].loc = locs[index];
                }

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += phys->dtj;

                size_t index = locs.size();
                double dutc = 50000.;
                for (uint16_t j=0; j<locs.size(); ++j)
                {
                    if (fabs(locs[j].utc - step[j].loc.utc) > dutc)
                    {
                        dutc = fabs(locs[j].utc - step[j].loc.utc);
                        index = j;
                    }
                }
                if (index < locs.size())
                {
                    step[i].loc = locs[index];
                }

                PosAccel(step[i].loc, *phys);
                AttAccel(step[i].loc, *phys);
            }
            iretn = Converge();
            phys->utc = loc->utc;
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Propagate()
        {
            Vector normal, unitv, unitx, unitp, unitp1, unitp2;
            Vector lunitp1(.1,.1,0.);
            Vector tvector;

            // Don't bother if too low
            if (Vector(loc->pos.eci.s).norm() < REARTHM)
            {
                return GENERAL_ERROR_TOO_LOW;
            }

            step[order+1].loc.utc = step[order+1].loc.pos.utc = step[order+1].loc.pos.eci.utc = step[order].loc.pos.eci.utc + phys->dtj;

            // Calculate S(order/2+1)
            step[order+1].ss.col[0] = step[order].ss.col[0] + step[order].s.col[0] + step[order].loc.pos.eci.a.col[0]/2.;
            step[order+1].ss.col[1] = step[order].ss.col[1] + step[order].s.col[1] + step[order].loc.pos.eci.a.col[1]/2.;
            step[order+1].ss.col[2] = step[order].ss.col[2] + step[order].s.col[2] + step[order].loc.pos.eci.a.col[2]/2.;

            // Calculate Sum(order/2+1) for a and b
            step[order+1].sb = step[order+1].sa = rv_zero();
            for (uint16_t k=0; k<=order; k++)
            {
                step[order+1].sb.col[0] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[0];
                step[order+1].sa.col[0] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[0];
                step[order+1].sb.col[1] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[1];
                step[order+1].sa.col[1] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[1];
                step[order+1].sb.col[2] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[2];
                step[order+1].sa.col[2] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[2];
            }

            // Calculate pos.v(order/2+1)
            step[order+1].loc.pos.eci.v.col[0] = phys->dt * (step[order].s.col[0] + step[order].loc.pos.eci.a.col[0]/2. + step[order+1].sb.col[0]);
            step[order+1].loc.pos.eci.v.col[1] = phys->dt * (step[order].s.col[1] + step[order].loc.pos.eci.a.col[1]/2. + step[order+1].sb.col[1]);
            step[order+1].loc.pos.eci.v.col[2] = phys->dt * (step[order].s.col[2] + step[order].loc.pos.eci.a.col[2]/2. + step[order+1].sb.col[2]);

            // Calculate pos.s(order/2+1)
            step[order+1].loc.pos.eci.s.col[0] = dtsq * (step[order+1].ss.col[0] + step[order+1].sa.col[0]);
            step[order+1].loc.pos.eci.s.col[1] = dtsq * (step[order+1].ss.col[1] + step[order+1].sa.col[1]);
            step[order+1].loc.pos.eci.s.col[2] = dtsq * (step[order+1].ss.col[2] + step[order+1].sa.col[2]);
            step[order+1].loc.pos.eci.pass++;
            pos_eci(step[order+1].loc);

            AttAccel(step[order+1].loc, *phys);
            PosAccel(step[order+1].loc, *phys);

            // Calculate s(order/2+1)
            step[order+1].s.col[0] = step[order].s.col[0] + (step[order].loc.pos.eci.a.col[0]+step[order+1].loc.pos.eci.a.col[0])/2.;
            step[order+1].s.col[1] = step[order].s.col[1] + (step[order].loc.pos.eci.a.col[1]+step[order+1].loc.pos.eci.a.col[1])/2.;
            step[order+1].s.col[2] = step[order].s.col[2] + (step[order].loc.pos.eci.a.col[2]+step[order+1].loc.pos.eci.a.col[2])/2.;

            // Shift everything over 1
            for (uint16_t j=0; j<=order; j++)
            {
                step[j] = step[j+1];
            }

            *loc = step[order].loc;

            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Converge()
        {
            uint32_t c_cnt, cflag=0, k;
            rvector oldsa;

            c_cnt = 0;
            do
            {
                step[order2].s.col[0] = step[order2].loc.pos.eci.v.col[0] / phys->dt;
                step[order2].s.col[1] = step[order2].loc.pos.eci.v.col[1] / phys->dt;
                step[order2].s.col[2] = step[order2].loc.pos.eci.v.col[2] / phys->dt;
                for (k=0; k<=order; k++)
                {
                    step[order2].s.col[0] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[0];
                    step[order2].s.col[1] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[1];
                    step[order2].s.col[2] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[2];
                }
                for (uint16_t n=1; n<=order2; n++)
                {
                    step[order2+n].s.col[0] = step[order2+n-1].s.col[0] + (step[order2+n].loc.pos.eci.a.col[0]+step[order2+n-1].loc.pos.eci.a.col[0])/2;
                    step[order2+n].s.col[1] = step[order2+n-1].s.col[1] + (step[order2+n].loc.pos.eci.a.col[1]+step[order2+n-1].loc.pos.eci.a.col[1])/2;
                    step[order2+n].s.col[2] = step[order2+n-1].s.col[2] + (step[order2+n].loc.pos.eci.a.col[2]+step[order2+n-1].loc.pos.eci.a.col[2])/2;
                    step[order2-n].s.col[0] = step[order2-n+1].s.col[0] - (step[order2-n].loc.pos.eci.a.col[0]+step[order2-n+1].loc.pos.eci.a.col[0])/2;
                    step[order2-n].s.col[1] = step[order2-n+1].s.col[1] - (step[order2-n].loc.pos.eci.a.col[1]+step[order2-n+1].loc.pos.eci.a.col[1])/2;
                    step[order2-n].s.col[2] = step[order2-n+1].s.col[2] - (step[order2-n].loc.pos.eci.a.col[2]+step[order2-n+1].loc.pos.eci.a.col[2])/2;
                }
                step[order2].ss.col[0] = step[order2].loc.pos.eci.s.col[0] / dtsq;
                step[order2].ss.col[1] = step[order2].loc.pos.eci.s.col[1] / dtsq;
                step[order2].ss.col[2] = step[order2].loc.pos.eci.s.col[2] / dtsq;
                for (k=0; k<=order; k++)
                {
                    step[order2].ss.col[0] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[0];
                    step[order2].ss.col[1] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[1];
                    step[order2].ss.col[2] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[2];
                }
                for (uint16_t n=1; n<=order2; n++)
                {
                    step[order2+n].ss.col[0] = step[order2+n-1].ss.col[0] + step[order2+n-1].s.col[0] + (step[order2+n-1].loc.pos.eci.a.col[0])/2;
                    step[order2+n].ss.col[1] = step[order2+n-1].ss.col[1] + step[order2+n-1].s.col[1] + (step[order2+n-1].loc.pos.eci.a.col[1])/2;
                    step[order2+n].ss.col[2] = step[order2+n-1].ss.col[2] + step[order2+n-1].s.col[2] + (step[order2+n-1].loc.pos.eci.a.col[2])/2;
                    step[order2-n].ss.col[0] = step[order2-n+1].ss.col[0] - step[order2-n+1].s.col[0] + (step[order2-n+1].loc.pos.eci.a.col[0])/2;
                    step[order2-n].ss.col[1] = step[order2-n+1].ss.col[1] - step[order2-n+1].s.col[1] + (step[order2-n+1].loc.pos.eci.a.col[1])/2;
                    step[order2-n].ss.col[2] = step[order2-n+1].ss.col[2] - step[order2-n+1].s.col[2] + (step[order2-n+1].loc.pos.eci.a.col[2])/2;
                }

                for (uint16_t n=0; n<=order; n++)
                {
                    if (n == order2)
                        continue;
                    step[n].sb = step[n].sa = rv_zero();
                    for (k=0; k<=order; k++)
                    {
                        step[n].sb.col[0] += step[n].b[k] * step[k].loc.pos.eci.a.col[0];
                        step[n].sa.col[0] += step[n].a[k] * step[k].loc.pos.eci.a.col[0];
                        step[n].sb.col[1] += step[n].b[k] * step[k].loc.pos.eci.a.col[1];
                        step[n].sa.col[1] += step[n].a[k] * step[k].loc.pos.eci.a.col[1];
                        step[n].sb.col[2] += step[n].b[k] * step[k].loc.pos.eci.a.col[2];
                        step[n].sa.col[2] += step[n].a[k] * step[k].loc.pos.eci.a.col[2];
                    }
                }

                for (uint16_t n=1; n<=order2; n++)
                {
                    for (int32_t i=-1; i<2; i+=2)
                    {
                        cflag = 0;

                        // Save current acceleration for comparison with next iteration
                        oldsa.col[0] = step[order2+i*n].loc.pos.eci.a.col[0];
                        oldsa.col[1] = step[order2+i*n].loc.pos.eci.a.col[1];
                        oldsa.col[2] = step[order2+i*n].loc.pos.eci.a.col[2];

                        // Calculate new probable position and velocity
                        step[order2+i*n].loc.pos.eci.v.col[0] = phys->dt * (step[order2+i*n].s.col[0] + step[order2+i*n].sb.col[0]);
                        step[order2+i*n].loc.pos.eci.v.col[1] = phys->dt * (step[order2+i*n].s.col[1] + step[order2+i*n].sb.col[1]);
                        step[order2+i*n].loc.pos.eci.v.col[2] = phys->dt * (step[order2+i*n].s.col[2] + step[order2+i*n].sb.col[2]);
                        step[order2+i*n].loc.pos.eci.s.col[0] = dtsq * (step[order2+i*n].ss.col[0] + step[order2+i*n].sa.col[0]);
                        step[order2+i*n].loc.pos.eci.s.col[1] = dtsq * (step[order2+i*n].ss.col[1] + step[order2+i*n].sa.col[1]);
                        step[order2+i*n].loc.pos.eci.s.col[2] = dtsq * (step[order2+i*n].ss.col[2] + step[order2+i*n].sa.col[2]);

                        // Perform conversions between different systems
                        step[order2+i*n].loc.pos.eci.pass++;
                        pos_eci(&step[order2+i*n].loc);
                        att_icrf2lvlh(&step[order2+i*n].loc);
                        //		eci2earth(&step[order2+i*n].loc.pos,&step[order2+i*n].att);

                        // Calculate acceleration at new position
                        PosAccel(step[order2+i*n].loc, *phys);

                        // Compare acceleration at new position to previous iteration
                        if (fabs(oldsa.col[0]-step[order2+i*n].loc.pos.eci.a.col[0])>1e-14 || fabs(oldsa.col[1]-step[order2+i*n].loc.pos.eci.a.col[1])>1e-14 || fabs(oldsa.col[2]-step[order2+i*n].loc.pos.eci.a.col[2])>1e-14)
                            cflag = 1;
                    }
                }
                c_cnt++;
            } while (c_cnt<10 && cflag);

            *loc = step[order].loc;
            ++loc->pos.eci.pass;
            pos_eci(loc);
            return 0;
        }

        //! Attitude acceleration
        /*! Calculate the torque forces on the specified satellite at the specified location/
            \param physics Pointer to structure specifying satellite.
            \param loc Structure specifying location.
        */
        int32_t AttAccel(locstruc &loc, physicsstruc &phys)
        {
            //    rvector ue, ta, tv;
            //    rvector ttorque;
            Vector ue, ta, tv;
            Vector ttorque;
            rmatrix mom;

            att_extra(loc);

            ttorque = phys.ctorque;

            // Now calculate Gravity Gradient Torque
            // Unit vector towards earth, rotated into body frame
            //    ue = irotate((loc.att.icrf.s),rv_smult(-1.,loc.pos.eci.s));
            //    normalize_rv(ue);
            ue = Quaternion(loc.att.icrf.s).irotate(-1. * Vector(loc.pos.eci.s)).normalize();

            //    phys.gtorque = rv_smult((3.*GM/pow(loc.pos.geos.s.r,3.)),rv_cross(ue,rv_mult(phys.moi,ue)));
            phys.gtorque = (3. * GM / pow(loc.pos.geos.s.r,3.)) * ue.cross(phys.moi * ue);

            //    ttorque = rv_add(ttorque,phys.gtorque);
            ttorque += phys.gtorque;

            // Atmospheric and solar torque
            //	ttorque = rv_add(ttorque,phys.atorque);
            //	ttorque = rv_add(ttorque,phys.rtorque);

            // Torque from rotational effects

            // Moment of Inertia in Body frame
            mom = rm_diag(phys.moi.to_rv());
            // Attitude rate in Body frame
            tv = irotate(loc.att.icrf.s,loc.att.icrf.v);

            // Torque from cross product of angular velocity and angular momentum
            //    phys.htorque = rv_smult(-1., rv_cross(tv,rv_add(rv_mmult(mom,tv),phys.hmomentum)));
            //    ttorque = rv_add(ttorque,phys.htorque);
            phys.htorque = -1. * tv.cross(Vector(rv_mmult(mom, tv.to_rv())) + phys.hmomentum);
            ttorque += phys.htorque;

            // Convert torque into accelerations, doing math in Body frame

            // I x alpha = tau, so alpha = I inverse x tau
            //    ta = rv_mmult(rm_inverse(mom),ttorque);
            ta = Vector(rv_mmult(rm_inverse(mom),ttorque.to_rv()));

            // Convert body frame acceleration back to other frames.
            loc.att.icrf.a = irotate(q_conjugate(loc.att.icrf.s), ta.to_rv());
            loc.att.topo.a = irotate(q_conjugate(loc.att.topo.s), ta.to_rv());
            loc.att.lvlh.a = irotate(q_conjugate(loc.att.lvlh.s), ta.to_rv());
            loc.att.geoc.a = irotate(q_conjugate(loc.att.geoc.s), ta.to_rv());
            loc.att.selc.a = irotate(q_conjugate(loc.att.selc.s), ta.to_rv());
            return 0;
        }

        static const uint8_t GravityPGM2000A = 1;
        static const uint8_t GravityEGM2008 = 2;
        static const uint8_t GravityPGM2000A_NORM = 3;
        static const uint8_t GravityEGM2008_NORM = 4;

        //! Position acceleration
        /*! Calculate the linear forces on the specified sattelite at the specified location/
            \param phys Pointer to structure specifying satellite.
            \param loc Structure specifying location.
        */
        int32_t PosAccel(locstruc &loc, physicsstruc &phys)
        {
            int32_t iretn;
            double radius;
            Vector ctpos, da, tda;
            cartpos bodypos;

            radius = length_rv(loc.pos.eci.s);

            loc.pos.eci.a = rv_zero();

            // Earth gravity
            // Calculate Geocentric acceleration vector

            if (radius > REARTHM)
            {
                // Start with gravity vector in ITRS

                da = GravityAccel(loc.pos,GravityEGM2008_NORM,12);

                // Correct for earth rotation, polar motion, precession, nutation

                da = Matrix(loc.pos.extra.e2j) * da;
            }
            else
            {
                // Simple 2 body
                da = -GM/(radius*radius*radius) * Vector(loc.pos.eci.s);
            }
            loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());

            // Sun gravity
            // Calculate Satellite to Sun vector
            ctpos = rv_sub(rv_smult(-1., loc.pos.extra.sun2earth.s), loc.pos.eci.s);
            radius = ctpos.norm();
            da = GSUN/(radius*radius*radius) * ctpos;
            loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());

            // Adjust for acceleration of frame
            radius = length_rv(loc.pos.extra.sun2earth.s);
            da = rv_smult(GSUN/(radius*radius*radius), loc.pos.extra.sun2earth.s);
            tda = da;
            loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());

            // Moon gravity
            // Calculate Satellite to Moon vector
            bodypos.s = rv_sub( loc.pos.extra.sun2earth.s, loc.pos.extra.sun2moon.s);
            ctpos = rv_sub(bodypos.s, loc.pos.eci.s);
            radius = ctpos.norm();
            da = GMOON/(radius*radius*radius) * ctpos;
            loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());

            // Adjust for acceleration of frame due to moon
            radius = length_rv(bodypos.s);
            da = rv_smult(GMOON/(radius*radius*radius),bodypos.s);
            tda -= da;
            loc.pos.eci.a = rv_sub(loc.pos.eci.a, da.to_rv());

            /*
        // Jupiter gravity
        // Calculate Satellite to Jupiter vector
        jplpos(JPL_EARTH,JPL_JUPITER, loc.pos.extra.tt,(cartpos *)&bodypos);
        ctpos = rv_sub(bodypos.s, loc.pos.eci.s);
        radius = length_rv(ctpos);

        // Calculate acceleration
        da = rv_smult(GJUPITER/(radius*radius*radius),ctpos);
        // loc.pos.eci.a = rv_add( loc.pos.eci.a,da);
        */

            // Atmospheric drag
            Quaternion iratt = Quaternion(loc.att.icrf.s).conjugate();
            if (phys.adrag.norm() > 0.)
            {
                da = iratt.irotate(phys.adrag);
                loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());
            }
            // Solar drag
            if (phys.rdrag.norm() > 0.)
            {
                da = iratt.irotate(phys.rdrag);
                loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());
            }
            // Fictitious drag
            if (phys.fdrag.norm() > 0.)
            {
                da = iratt.irotate(phys.fdrag);
                loc.pos.eci.a = rv_add(loc.pos.eci.a, da.to_rv());
            }

            loc.pos.eci.pass++;
            iretn = pos_eci(&loc);
            if (iretn < 0)
            {
                return iretn;
            }
            if (std::isnan( loc.pos.eci.a.col[0]))
            {
                loc.pos.eci.a.col[0] = 0.;
            }
            return 0;
        }

        //! Calculate atmospheric density
        /*! Calculate atmospheric density at indicated Latitute/Longitude/Altitude using the
         * NRLMSISE-00 atmospheric model.
            \param pos Structure indicating position
            \param f107avg Average 10.7 cm solar flux
            \param f107 Current 10.7 cm solar flux
            \param magidx Ap daily geomagnetic index
            \return Density in kg/m3
        */
        double Msis00Density(posstruc pos, float f107avg, float f107, float magidx)
        {
            struct nrlmsise_output output;
            struct nrlmsise_input input;
            struct nrlmsise_flags flags = {
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
            {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.}};
            int year, month;
            double day, doy;
            static double lastmjd = 0.;
            static double lastdensity = 0.;
            static double lastperiod = 0.;

            if (lastperiod != 0.)
            {
                if (fabs(lastperiod) > (pos.extra.utc-lastmjd))
                {
                    return (lastdensity*(1.+(.001*(pos.extra.utc-lastmjd)/lastperiod)));
                }
            }

            mjd2ymd(pos.extra.utc,year,month,day,doy);
            input.doy = static_cast <int32_t>(doy);
            input.g_lat = pos.geod.s.lat*180./DPI;
            input.g_long = pos.geod.s.lon*180./DPI;
            input.alt = pos.geod.s.h / 1000.;
            input.sec = (doy - input.doy)*86400.;;
            input.lst = input.sec / 3600. + input.g_long / 15.;
            input.f107A = f107avg;
            input.f107 = f107;
            input.ap = magidx;
            gtd7d(&input,&flags,&output);

            if (lastdensity != 0. && lastdensity != output.d[5])
                lastperiod = (pos.extra.utc-lastmjd)*.001*output.d[5]/(output.d[5]-lastdensity);
            lastmjd = pos.extra.utc;
            lastdensity = output.d[5];
            return((double)output.d[5]);
        }

        //! Data structures for spherical harmonic expansion
        /*! Coefficients for real and imaginary components of expansion. Of order and rank maxdegree
        */
        static const uint16_t maxdegree = 360;
        static double vc[maxdegree+1][maxdegree+1], wc[maxdegree+1][maxdegree+1];
        static double coef[maxdegree+1][maxdegree+1][2];
        static double ftl[2*maxdegree+1];
        static double spmm[maxdegree+1];

        //! Spherical harmonic  gravitational vector
        /*!
        * Calculates a spherical harmonic expansion of the chosen model of indicated order and
        * degree for the requested position.
        * The result is returned as a geocentric vector calculated at the epoch.
            \param pos a ::posstruc providing the position at the epoch
            \param model Model to use for coefficients
            \param degree Order and degree to calculate
            \return A ::Vector pointing toward the earth
            \see pgm2000a_coef.txt
        */
        Vector GravityAccel(posstruc pos, uint16_t model, uint32_t degree)
        {
            uint32_t il, im;
            double tmult;
            double ratio, rratio, xratio, yratio, zratio;
            Vector accel;
            double fr;

            // Zero out vc and wc
            memset(vc,0,sizeof(vc));
            memset(wc,0,sizeof(wc));

            // Load Params
            GravityParams(model);

            // Calculate cartesian Legendre terms
            vc[0][0] = REARTHM/pos.geos.s.r;
            wc[0][0] = 0.;
            ratio = vc[0][0] / pos.geos.s.r;
            rratio = REARTHM * ratio;
            xratio = pos.geoc.s.col[0] * ratio;
            yratio = pos.geoc.s.col[1] * ratio;
            zratio = pos.geoc.s.col[2] * ratio;
            vc[1][0] = zratio * vc[0][0];
            wc[1][0] = 0.;
            for (il=2; il<=degree+1; il++)
            {
                vc[il][0] = (2*il-1)*zratio * vc[il-1][0] / il - (il-1) * rratio * vc[il-2][0] / il;
                wc[il][0] = 0.;
            }
            for (im=1; im<=degree+1; im++)
            {
                vc[im][im] = (2*im-1) * (xratio * vc[im-1][im-1] - yratio * wc[im-1][im-1]);
                wc[im][im] = (2*im-1) * (xratio * wc[im-1][im-1] + yratio * vc[im-1][im-1]);
                if (im <= degree)
                {
                    vc[im+1][im] = (2*im+1) * zratio * vc[im][im];
                    wc[im+1][im] = (2*im+1) * zratio * wc[im][im];
                }
                for (il=im+2; il<=degree+1; il++)
                {
                    vc[il][im] = (2*il-1) * zratio * vc[il-1][im] / (il-im) - (il+im-1) * rratio * vc[il-2][im] / (il-im);
                    wc[il][im] = (2*il-1) * zratio * wc[il-1][im] / (il-im) - (il+im-1) * rratio * wc[il-2][im] / (il-im);
                }
            }

            //	dr = dlon = dlat = 0.;

            accel.clear();
            for (im=0; im<=degree; im++)
            {
                for (il=im; il<=degree; il++)
                {
                    if (im == 0)
                    {
                        accel[0] -= coef[il][0][0] * vc[il+1][1];
                        accel[1] -= coef[il][0][0] * wc[il+1][1];
                        accel[2] -= (il+1) * (coef[il][0][0] * vc[il+1][0]);
                    }
                    else
                    {
                        fr = ftl[il-im+2] / ftl[il-im];
                        accel[0] -= .5 * (coef[il][im][0] * vc[il+1][im+1] + coef[il][im][1] * wc[il+1][im+1] - fr * (coef[il][im][0] * vc[il+1][im-1] + coef[il][im][1] * wc[il+1][im-1]));
                        accel[1] -= .5 * (coef[il][im][0] * wc[il+1][im+1] - coef[il][im][1] * vc[il+1][im+1] + fr * (coef[il][im][0] * wc[il+1][im-1] - coef[il][im][1] * vc[il+1][im-1]));
                        accel[2] -= (il-im+1) * (coef[il][im][0] * vc[il+1][im] + coef[il][im][1] * wc[il+1][im]);
                    }
                }
            }
            tmult = GM / (REARTHM*REARTHM);
            accel[0] *= tmult;
            accel[2] *= tmult;
            accel[1] *= tmult;

            return (accel);
        }

        int32_t GravityParams(int16_t model)
        {
            static int16_t cmodel = -1;

            int32_t iretn = 0;
            uint32_t il, im;
            double norm;
            uint32_t dil, dim;
            double dummy1, dummy2;

            // Calculate factorial
            if (ftl[0] == 0.)
            {
                ftl[0] = 1.;
                for (il=1; il<2*maxdegree+1; il++)
                {
                    ftl[il] = il * ftl[il-1];
                }
            }

            // Load Coefficients
            if (cmodel != model)
            {
                coef[0][0][0] = 1.;
                coef[0][0][1] = 0.;
                coef[1][0][0] = coef[1][0][1] = 0.;
                coef[1][1][0] = coef[1][1][1] = 0.;
                string fname;
                FILE *fi;
                switch (model)
                {
                case GravityEGM2008:
                case GravityEGM2008_NORM:
                    fname = get_cosmosresources();
                    if (fname.empty())
                    {
                        return GENERAL_ERROR_EMPTY;
                    }
                    fname += "/general/egm2008_coef.txt";
                    fi = fopen(fname.c_str(),"r");

                    if (fi==nullptr)
                    {
                        cout << "could not load file " << fname << endl;
                        return iretn;
                    }

                    for (il=2; il<101; il++)
                    {
                        for (im=0; im<= il; im++)
                        {
                            iretn = fscanf(fi,"%u %u %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1]);
                            if (iretn && model == GravityEGM2008_NORM)
                            {
                                norm = sqrt(ftl[il+im]/((2-(im==0?1:0))*(2*il+1)*ftl[il-im]));
                                coef[il][im][0] /= norm;
                                coef[il][im][1] /= norm;
                            }
                        }
                    }
                    fclose(fi);
                    cmodel = model;
                    break;
                case GravityPGM2000A:
                case GravityPGM2000A_NORM:
                default:
                    iretn = get_cosmosresources(fname);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    fname += "/general/pgm2000a_coef.txt";
                    fi = fopen(fname.c_str(),"r");
                    for (il=2; il<361; il++)
                    {
                        for (im=0; im<= il; im++)
                        {
                            iretn = fscanf(fi,"%u %u %lf %lf %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1],&dummy1,&dummy2);
                            if (iretn && model == GravityPGM2000A_NORM)
                            {
                                norm = sqrt(ftl[il+im]/((2-(il==im?1:0))*(2*il+1)*ftl[il-im]));
                                coef[il][im][0] /= norm;
                                coef[il][im][1] /= norm;
                            }
                        }
                    }
                    fclose(fi);
                    cmodel = model;
                    break;
                }
            }
            return 0;
        }

        double nplgndr(uint32_t l, uint32_t m, double x)
        {
            double fact,pll,pmm,pmmp1,omx2, oldfact;
            uint16_t i, ll, mm;
            static double lastx = 10.;
            static uint16_t lastm = 65535;

            pll = 0.;
            if (lastm == 65535 || m > lastm || x != lastx)
            {
                lastx = x;
                lastm = m;
                mm = m;
                //	for (mm=0; mm<=maxdegree; mm++)
                //		{
                pmm=1.0;
                if (mm > 0)
                {
                    omx2=((1.0-x)*(1.0+x));
                    fact=1.0;
                    for (i=1;i<=mm;i++)
                    {
                        pmm *= fact*omx2/(fact+1.);
                        fact += 2.0;
                    }
                }
                pmm = sqrt((2.*m+1.)*pmm);
                if (mm%2 == 1)
                    pmm = - pmm;
                spmm[mm] = pmm;
                //		}
            }

            pmm = spmm[m];
            if (l == m)
                return pmm;
            else {
                pmmp1=x*sqrt(2.*m+3.)*pmm;
                if (l == (m+1))
                    return pmmp1;
                else {
                    oldfact = sqrt(2.*m+3.);
                    for (ll=m+2;ll<=l;ll++)
                    {
                        fact = sqrt((4.*ll*ll-1.)/(ll*ll-m*m));
                        pll=(x*pmmp1-pmm/oldfact)*fact;
                        oldfact = fact;
                        pmm=pmmp1;
                        pmmp1=pll;
                    }
                    return pll;
                }
            }
        }

        int32_t InitializePhysics(physicsstruc &physics, float imass, float itemp, float ibatt, Vector imoi, float ihcap, double idt)
        {
            physics.dt = idt;
            physics.dtj = physics.dt / 86400.;
            physics.mass = imass;
            physics.temp = itemp;
            physics.hcap = ihcap;
            physics.heat = physics.mass * physics.temp * physics.hcap;
            physics.moi = imoi;
            physics.battcap = ibatt;
            physics.battlev = ibatt / 2.f;

            physics.powgen = 0.;
            physics.powuse = 0.;
            physics.ftorque = Vector();
            physics.atorque = Vector();
            physics.rtorque = Vector();
            physics.gtorque = Vector();
            physics.htorque = Vector();
            physics.ctorque = Vector();
            physics.hmomentum = Vector();
            physics.fdrag = Vector();
            physics.adrag = Vector();
            physics.rdrag = Vector();
            physics.thrust = Vector();

            physics.com = Vector();
            physics.area = 0.;

            return 0;
        }

        int32_t UpdatePhysics(physicsstruc &phys, locstruc &location)
        {
            //            phys.heat = 0.;
            phys.rtorque.clear();
            phys.rdrag.clear();
            phys.adrag.clear();

            Vector unitv = Quaternion(location.att.geoc.s).irotate(Vector(location.pos.geoc.v).normalize());

            double speed = location.pos.geoc.v.col[0]*location.pos.geoc.v.col[0]+location.pos.geoc.v.col[1]*location.pos.geoc.v.col[1]+location.pos.geoc.v.col[2]*location.pos.geoc.v.col[2];
            double density;
            if (location.pos.geod.s.h < 10000. || std::isnan(location.pos.geod.s.h))
            {
                density = 1.225;
            }
            else
            {
                density = 1000. * Msis00Density(location.pos,150.,150.,3.);
            }
            double adrag = density * 1.1 * speed;

            Vector unite = Vector(location.pos.eci.s).normalize(-1.);
            unite = Vector(location.pos.eci.s).normalize(-1.);
            unite = Quaternion(location.att.icrf.s).irotate(Vector(location.pos.eci.s).normalize(-1.));
            Vector units = Quaternion(location.att.icrf.s).irotate(Vector(location.pos.icrf.s).normalize());
            for (uint16_t i=0; i<phys.triangles.size(); ++i)
            {
                phys.triangles[i].irradiation = 0.;
                if (phys.triangles[i].external)
                {
                    // Solar Radiation effects
                    double sdot = units.dot(phys.triangles[i].normal);
                    if (location.pos.sunradiance > 0.f && sdot > 0.)
                    {
                        double ddrag = location.pos.sunradiance * sdot / (3e8*phys.mass);
                        Vector dtorque = ddrag * phys.triangles[i].twist;
                        phys.rtorque += dtorque;
                        Vector da = ddrag * phys.triangles[i].shove;
                        phys.rdrag += da;

                        phys.triangles[i].irradiation += location.pos.sunradiance * sdot / phys.triangles[i].normal.norm();
                    }

                    // Earth radiaion effects
                    double edot = acos(unite.dot(phys.triangles[i].normal) / phys.triangles[i].normal.norm()) - RADOF(5.);
                    if (edot < 0.)
                        edot = 1.;
                    else
                        edot = cos(edot);
                    if (edot > 0)
                    {
                        phys.triangles[i].irradiation += phys.triangles[i].abs*phys.triangles[i].area * edot * phys.dt * SIGMA * pow(290.,4);
                    }

                    // Atmospheric effects
                    double vdot = unitv.dot(phys.triangles[i].normal);
                    if (vdot > 0)
                    {
                        double ddrag;
                        if (phys.mass)
                        {
                            ddrag = adrag*vdot/phys.mass;
                        }
                        else
                        {
                            ddrag = 0.;
                        }
                        phys.atorque += ddrag * phys.triangles[i].twist;
                        phys.adrag += ddrag * phys.triangles[i].shove;
                    }
                }
            }
            return 0;
        }

        int32_t InitializeStructure(physicsstruc &physics, StructureType type, float abs, float emi)
        {
            trianglestruc ttriangle;

            physics.triangles.clear();
            physics.area = 0.;
            switch (type)
            {
            case None:
                break;
            case Cube1a:
                {
                    // 1U with cells on all sides
                    double dx = .05;
                    double dy = .05;
                    double dz = .05;

                    // -X
                    physics.vertices.push_back(Vector(-dx, -dy, -dz));
                    physics.vertices.push_back(Vector(-dx, -dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, -dz));
                    physics.vertices.push_back(Vector(-dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 2;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 2;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +X
                    physics.vertices.push_back(Vector(dx, -dy, -dz));
                    physics.vertices.push_back(Vector(dx, -dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, -dz));
                    physics.vertices.push_back(Vector(dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Y
                    physics.vertices.push_back(Vector(0., -dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Y
                    physics.vertices.push_back(Vector(0., dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Z
                    physics.vertices.push_back(Vector(0., 0., -dz));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Z
                    physics.vertices.push_back(Vector(0., 0., dz));

                    // triangle 1
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // Decks
                    for (double ddx=-dx + .1; dx < dx; dx+=.1)
                    {
                        physics.vertices.push_back(Vector(-ddx, -dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, -dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, 0., 0.));

                        // triangle 1
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 2
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 3
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 4
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;
                    }
                }
                break;
            case Cube3a:
                {
                    // 3U with cells on all sides
                    double dx = .15;
                    double dy = .05;
                    double dz = .05;

                    // -X
                    physics.vertices.push_back(Vector(-dx, -dy, -dz));
                    physics.vertices.push_back(Vector(-dx, -dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, -dz));
                    physics.vertices.push_back(Vector(-dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 2;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 2;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +X
                    physics.vertices.push_back(Vector(dx, -dy, -dz));
                    physics.vertices.push_back(Vector(dx, -dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, -dz));
                    physics.vertices.push_back(Vector(dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Y
                    physics.vertices.push_back(Vector(0., -dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Y
                    physics.vertices.push_back(Vector(0., dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Z
                    physics.vertices.push_back(Vector(0., 0., -dz));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Z
                    physics.vertices.push_back(Vector(0., 0., dz));

                    // triangle 1
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // Decks
                    for (double ddx=-dx + .1; dx < dx; dx+=.1)
                    {
                        physics.vertices.push_back(Vector(-ddx, -dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, -dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, 0., 0.));

                        // triangle 1
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 2
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 3
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 4
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;
                    }
                }
                break;
            case Cube6a:
                {
                    // 6U with cells on all sides
                    double dx = .15;
                    double dy = .05;
                    double dz = .15;

                    // -X
                    physics.vertices.push_back(Vector(-dx, -dy, -dz));
                    physics.vertices.push_back(Vector(-dx, -dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, dz));
                    physics.vertices.push_back(Vector(-dx, dy, -dz));
                    physics.vertices.push_back(Vector(-dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 2;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 2;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 4;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +X
                    physics.vertices.push_back(Vector(dx, -dy, -dz));
                    physics.vertices.push_back(Vector(dx, -dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, dz));
                    physics.vertices.push_back(Vector(dx, dy, -dz));
                    physics.vertices.push_back(Vector(dx, 0., 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 9;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Y
                    physics.vertices.push_back(Vector(0., -dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 10;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Y
                    physics.vertices.push_back(Vector(0., dy, 0.));

                    // triangle 1
                    ttriangle.tidx[0] = 8;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // -Z
                    physics.vertices.push_back(Vector(0., 0., -dz));

                    // triangle 1
                    ttriangle.tidx[0] = 5;
                    ttriangle.tidx[1] = 0;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 0;
                    ttriangle.tidx[1] = 3;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 3;
                    ttriangle.tidx[1] = 8;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 5;
                    ttriangle.tidx[2] = 11;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // +Z
                    physics.vertices.push_back(Vector(0., 0., dz));

                    // triangle 1
                    ttriangle.tidx[0] = 6;
                    ttriangle.tidx[1] = 1;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 2
                    ttriangle.tidx[0] = 1;
                    ttriangle.tidx[1] = 9;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 3
                    ttriangle.tidx[0] = 9;
                    ttriangle.tidx[1] = 7;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // triangle 4
                    ttriangle.tidx[0] = 7;
                    ttriangle.tidx[1] = 6;
                    ttriangle.tidx[2] = 12;
                    ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                    ttriangle.normal = physics.vertices[ttriangle.tidx[2]];
                    ttriangle.pcell = .8f;
                    ttriangle.abs = abs;
                    ttriangle.emi = emi;
                    physics.triangles.push_back(ttriangle);
                    physics.area += ttriangle.area;

                    // Decks
                    for (double ddx=-dx + .1; dx < dx; dx+=.1)
                    {
                        physics.vertices.push_back(Vector(-ddx, -dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, -dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, dz));
                        physics.vertices.push_back(Vector(-ddx, dy, -dz));
                        physics.vertices.push_back(Vector(-ddx, 0., 0.));

                        // triangle 1
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 2
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 1;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 3
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 2;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;

                        // triangle 4
                        ttriangle.tidx[0] = (physics.vertices.size()-5) + 3;
                        ttriangle.tidx[1] = (physics.vertices.size()-5) + 0;
                        ttriangle.tidx[2] = (physics.vertices.size()-5) + 4;
                        ttriangle.com = physics.vertices[ttriangle.tidx[0]].cross(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.area = physics.vertices[ttriangle.tidx[0]].area(physics.vertices[ttriangle.tidx[1]]);
                        ttriangle.normal = (Vector(1., 0., 0.));
                        physics.triangles.push_back(ttriangle);
                        physics.area += ttriangle.area;
                    }
                }
                break;
            case Cube6b:
                break;
            }

            for (uint16_t i=0; i<physics.triangles.size(); ++i)
            {
                physics.triangles[i].perimeter = (physics.vertices[ttriangle.tidx[0]] - physics.vertices[ttriangle.tidx[1]]).norm();
                physics.triangles[i].perimeter += (physics.vertices[ttriangle.tidx[1]] - physics.vertices[ttriangle.tidx[2]]).norm();
                physics.triangles[i].perimeter += (physics.vertices[ttriangle.tidx[2]] - physics.vertices[ttriangle.tidx[0]]).norm();
                physics.triangles[i].mass = physics.triangles[i].area * physics.mass / physics.area;
                physics.triangles[i].heat = physics.triangles[i].area * physics.heat / physics.area;
                physics.triangles[i].hcap = physics.hcap;
                physics.triangles[i].temp = physics.triangles[i].heat / (physics.triangles[i].mass * physics.triangles[i].hcap);
                physics.triangles[i].shove = -physics.triangles[i].area * (physics.triangles[i].normal.dot(physics.triangles[i].com)) * physics.triangles[i].com / (physics.triangles[i].com.norm() * physics.triangles[i].com.norm());
                physics.triangles[i].twist = -physics.triangles[i].area * physics.triangles[i].com.norm() * physics.triangles[i].normal - physics.triangles[i].com.norm() * physics.triangles[i].shove;
            }

            return physics.triangles.size();
        }

        int32_t UpdateStructure(physicsstruc &physics)
        {
            for (uint16_t i=0; i<physics.triangles.size(); ++i)
            {
                physics.triangles[i].mass = physics.triangles[i].area * physics.mass / physics.area;
                physics.triangles[i].heat = physics.triangles[i].area * physics.heat / physics.area;
                physics.triangles[i].temp = physics.triangles[i].heat / (physics.triangles[i].mass * physics.triangles[i].hcap);
            }

            return 0;
        }

    }

}