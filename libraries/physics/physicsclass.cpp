#include "physicsclass.h"

namespace Cosmos
{
    namespace Physics
    {
        int32_t Structure::Setup(uint16_t type)
        {
            return 0;
        }

        int32_t Structure::add_u(double x, double y, double z, ExternalPanelType type)
        {
            switch (type)
            {
            case None:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .004);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .004);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(-x/2., -y/2., -z/2.), .004);
                break;
            case X:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .004);
                add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .004);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .004);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(-x/2., -y/2., -z/2.), .004);
                break;
            case Y:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .004);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .004);
                add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .004);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(-x/2., -y/2., -z/2.), .004);
                break;
            case XY:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .004);
                add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .004);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004, Math::Quaternions::eye(), Vector(), true, 0.);
                add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .004);
                add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .004);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., -y/2., z/2.), .004);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(-x/2., -y/2., -z/2.), .004);
                break;
            }

            return 0;
        }

        int32_t Structure::add_cuboid(string name, Vector size, double depth, Quaternion orientation, Vector offset)
        {
            add_face(name+"+x", Vector(size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), depth, orientation, offset, false);
            add_face(name+"-x", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., -size.y/2., size.z/2.), depth, orientation, offset, false);
            add_face(name+"+y", Vector(-size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), depth, orientation, offset, false);
            add_face(name+"-y", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., -size.y/2., size.z/2.), depth, orientation, offset, false);
            add_face(name+"+z", Vector(-size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., -size.y/2., size.z/2.), depth, orientation, offset, false);
            add_face(name+"-z", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(-size.x/2., -size.y/2., -size.z/2.), depth, orientation, offset, false);

            return 0;
        }

        int32_t Structure::add_face(string name, Vector point0, Vector point1, Vector point2, Vector point3, double depth, Quaternion orientation, Vector offset, bool external, float pcell)
        {
            point0 = orientation.irotate(point0);
            point0 += offset;
            point1 = orientation.irotate(point1);
            point1 += offset;
            point2 = orientation.irotate(point2);
            point2 += offset;
            point3 = orientation.irotate(point3);
            point3 += offset;
            Vector point4 = (point0 + point1 + point2 + point3) / 4.;

            add_triangle(point0, point1, point4, depth, external, pcell);
            add_triangle(point1, point2, point4, depth, external, pcell);
            add_triangle(point2, point3, point4, depth, external, pcell);
            add_triangle(point3, point0, point4, depth, external, pcell);

            return 0;
        }

        int32_t Structure::add_face(string name, Vector size, Quaternion orientation, Vector offset)
        {
            Vector points[5];

            points[0].x = -size.x / 2.;
            points[2].x = -size.x / 2.;
            points[1].x = size.x / 2.;
            points[3].x = size.x / 2.;

            points[0].y = -size.y / 2.;
            points[1].y = -size.y / 2.;
            points[2].y = size.y / 2.;
            points[3].y = size.y / 2.;

            for (uint16_t i=0; i<5; ++i)
            {
                points[i] = orientation.irotate(points[i]);
                points[i] += offset;
            }

            add_triangle(points[0], points[1], points[4], size.z);
            add_triangle(points[1], points[1], points[4], size.z);
            add_triangle(points[2], points[3], points[4], size.z);
            add_triangle(points[3], points[0], points[4], size.z);

            return 4;
        }

        int32_t Structure::add_triangle(Vector pointa, Vector pointb, Vector pointc, double depth, bool external, float pcell)
        {
            trianglestruc triangle;

            triangle.tidx[0] = add_vertex(pointa);
            triangle.tidx[1] = add_vertex(pointb);
            triangle.tidx[2] = add_vertex(pointc);

            triangle.external = external;
            triangle.depth = depth;
            triangle.pcell = pcell;
            triangle.normal = (phys->vertices[triangle.tidx[1]] - phys->vertices[triangle.tidx[0]]).cross(phys->vertices[triangle.tidx[2]] - phys->vertices[triangle.tidx[0]]);
            triangle.com = (phys->vertices[triangle.tidx[0]] + phys->vertices[triangle.tidx[1]] + phys->vertices[triangle.tidx[2]]) / 3.;
            triangle.area = (phys->vertices[triangle.tidx[1]] - phys->vertices[triangle.tidx[0]]).area(phys->vertices[triangle.tidx[2]] - phys->vertices[triangle.tidx[0]]);
            triangle.perimeter = (phys->vertices[triangle.tidx[1]] - phys->vertices[triangle.tidx[0]]).norm() + (phys->vertices[triangle.tidx[2]] - phys->vertices[triangle.tidx[1]]).norm() + (phys->vertices[triangle.tidx[0]] - phys->vertices[triangle.tidx[2]]).norm();
            phys->triangles.push_back(triangle);

            return 1;
        }

        int32_t Structure::add_vertex(Vector point)
        {
            bool found = false;

            int32_t index = -1;
            for (uint16_t i=0; i<phys->vertices.size(); ++ i)
            {
                if ((point - phys->vertices[i]).norm() < .001)
                {
                    found = true;
                    index = i;
                    break;
                }
            }

            if (found)
            {
                return index;
            }
            else {
                phys->vertices.push_back(point);
                return phys->vertices.size() - 1;
            }
        }

        State::State()
        {
        }

        int32_t State::Init(Propagator *posprop, Propagator *attprop, Propagator *thermprop, Propagator *elecprop)
        {
            int32_t iretn = 0;

            oldloc = newloc;
            oldphys = newphys;

            position = posprop;
            attitude = attprop;
            thermal = thermprop;
            electrical = elecprop;

            newphys = position->newphys;
            newloc = position->newloc;
            dt = position->dt;
            dtj = position->dtj;

            return iretn;
        }

        int32_t State::Increment(double nextutc)
        {
            int32_t count = 0;
            do
            {
                newloc->utc += dtj;
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
            } while (newloc->utc < nextutc);
            return count;
        }


        int32_t InertialAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t InertialAttitudePropagator::Propagate()
        {
            oldloc->att = newloc->att;
            AttAccel(*newloc, *newphys);

            return 0;
        }

        int32_t IterativeAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t IterativeAttitudePropagator::Propagate()
        {
            quaternion q1;

            oldloc->att = newloc->att;

            q1 = q_axis2quaternion_rv(rv_smult(dt, newloc->att.icrf.v));
            newloc->att.icrf.s = q_fmult(q1, newloc->att.icrf.s);
            normalize_q(&newloc->att.icrf.s);
            // Calculate new v from da
            newloc->att.icrf.v = rv_add(newloc->att.icrf.v, rv_smult(dt, newloc->att.icrf.a));
            newloc->att.icrf.utc = newloc->utc;
            att_icrf(newloc);
            AttAccel(*newloc, *newphys);

            return 0;
        }

        int32_t LVLHAttitudePropagator::Init()
        {
            newloc->att.lvlh.utc = newloc->utc;
            newloc->att.lvlh.s = q_eye();
            newloc->att.lvlh.v = rv_zero();
            newloc->att.lvlh.a = rv_zero();
            ++newloc->att.lvlh.pass;
            att_lvlh(newloc);
            AttAccel(*newloc, *newphys);

            return  0;
        }

        int32_t LVLHAttitudePropagator::Propagate()
        {
            oldloc->att = newloc->att;

            newloc->att.lvlh.utc = newloc->utc;
            newloc->att.lvlh.s = q_eye();
            newloc->att.lvlh.v = rv_zero();
            newloc->att.lvlh.a = rv_zero();
            ++newloc->att.lvlh.pass;
            att_lvlh(newloc);
            AttAccel(*newloc, *newphys);

            return 0;
        }

        int32_t ThermalPropagator::Init(float temp)
        {
            temperature = temp;
            return 0;
        }

        int32_t ThermalPropagator::Propagate()
        {
            return 0;
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
            oldloc = newloc;
            oldphys = newphys;
            newloc->pos.eci.pass++;
            pos_eci(newloc);
            PosAccel(*newloc, *newphys);

            return 0;
        }

        int32_t IterativePositionPropagator::Init()
        {
            return 0;
        }

        int32_t IterativePositionPropagator::Propagate()
        {
            oldloc = newloc;
            oldphys = newphys;
            rvector ds = rv_smult(.5 * dt * dt, newloc->pos.eci.a);
            ds = rv_add(ds, rv_smult(dt, newloc->pos.eci.v));
            newloc->pos.eci.s = rv_add(newloc->pos.eci.s, ds);
            newloc->pos.eci.v = rv_add(newloc->pos.eci.v, rv_smult(dt, newloc->pos.eci.a));
            newloc->pos.eci.pass++;
            pos_eci(newloc);
            PosAccel(*newloc, *newphys);

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

            dtsq = dt * dt;

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

            oldloc = newloc;
            oldphys = newphys;
            loc_clear(step[order+1].loc);
            lines2eci(newloc->utc, lines, newloc->pos.eci);
            ++newloc->pos.eci.pass;
            pos_eci(newloc);
            PosAccel(*newloc, *newphys);
            AttAccel(*newloc, *newphys);
            step[order2].loc = *newloc;

            // Position at t0-dt
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i+1].loc.att.lvlh;
                att_lvlh2icrf(step[i].loc);

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i].loc = step[i-1].loc;

                step[i].loc.utc += dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i-1].loc.att.lvlh;
                att_lvlh2icrf(step[i].loc);

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }

            iretn = Converge();
            newphys->utc = newloc->utc;

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
            ++newloc->pos.eci.pass;
            pos_eci(newloc);

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);

            // Calculate initial accelerations
            PosAccel(*newloc, *newphys);
            AttAccel(*newloc, *newphys);

            // Set central bin to initial state vector
            step[order2].loc = *newloc;

            // Position at t0-dt
            eci2kep(newloc->pos.eci, kep);

            // Initialize past bins
            for (i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;
                kep.utc = step[i].loc.utc;
                kep.ma -= dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(-dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(-dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                pos_eci(step[i].loc);

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }

            eci2kep(newloc->pos.eci, kep);
            for (i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += dtj;
                kep.utc = step[i].loc.utc;
                kep.ma += dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                pos_eci(step[i].loc);

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }
            iretn = Converge();
            newphys->utc = newloc->utc;
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Init(vector<locstruc> locs)
        {
            int32_t iretn = 0;

            // Make sure ::locstruc is internally self consistent
            ++newloc->pos.eci.pass;
            pos_eci(newloc);

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);

            // Calculate initial accelerations
            PosAccel(*newloc, *newphys);
            AttAccel(*newloc, *newphys);

            // Set central bin to initial state vector
            step[order2].loc = *newloc;

            // Initialize past bins
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;

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

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += dtj;

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

                PosAccel(step[i].loc, *newphys);
                AttAccel(step[i].loc, *newphys);
            }
            iretn = Converge();
            newphys->utc = newloc->utc;
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Propagate()
        {
            Vector normal, unitv, unitx, unitp, unitp1, unitp2;
            Vector lunitp1(.1,.1,0.);
            Vector tvector;

            oldloc = newloc;
            oldphys = newphys;

            // Don't bother if too low
            if (Vector(newloc->pos.eci.s).norm() < REARTHM)
            {
                return GENERAL_ERROR_TOO_LOW;
            }

            step[order+1].loc.utc = step[order+1].loc.pos.utc = step[order+1].loc.pos.eci.utc = step[order].loc.pos.eci.utc + dtj;

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
            step[order+1].loc.pos.eci.v.col[0] = this->dt * (step[order].s.col[0] + step[order].loc.pos.eci.a.col[0]/2. + step[order+1].sb.col[0]);
            step[order+1].loc.pos.eci.v.col[1] = this->dt * (step[order].s.col[1] + step[order].loc.pos.eci.a.col[1]/2. + step[order+1].sb.col[1]);
            step[order+1].loc.pos.eci.v.col[2] = this->dt * (step[order].s.col[2] + step[order].loc.pos.eci.a.col[2]/2. + step[order+1].sb.col[2]);

            // Calculate pos.s(order/2+1)
            step[order+1].loc.pos.eci.s.col[0] = this->dtsq * (step[order+1].ss.col[0] + step[order+1].sa.col[0]);
            step[order+1].loc.pos.eci.s.col[1] = this->dtsq * (step[order+1].ss.col[1] + step[order+1].sa.col[1]);
            step[order+1].loc.pos.eci.s.col[2] = this->dtsq * (step[order+1].ss.col[2] + step[order+1].sa.col[2]);
            step[order+1].loc.pos.eci.pass++;
            pos_eci(step[order+1].loc);

            AttAccel(step[order+1].loc, *newphys);
            PosAccel(step[order+1].loc, *newphys);

            // Calculate s(order/2+1)
            step[order+1].s.col[0] = step[order].s.col[0] + (step[order].loc.pos.eci.a.col[0]+step[order+1].loc.pos.eci.a.col[0])/2.;
            step[order+1].s.col[1] = step[order].s.col[1] + (step[order].loc.pos.eci.a.col[1]+step[order+1].loc.pos.eci.a.col[1])/2.;
            step[order+1].s.col[2] = step[order].s.col[2] + (step[order].loc.pos.eci.a.col[2]+step[order+1].loc.pos.eci.a.col[2])/2.;

            // Shift everything over 1
            for (uint16_t j=0; j<=order; j++)
            {
                step[j] = step[j+1];
            }

            *newloc = step[order].loc;

            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Converge()
        {
            uint32_t c_cnt, cflag=0, k;
            rvector oldsa;

            oldloc = newloc;
            oldphys = newphys;

            c_cnt = 0;
            do
            {
                step[order2].s.col[0] = step[order2].loc.pos.eci.v.col[0]/this->dt;
                step[order2].s.col[1] = step[order2].loc.pos.eci.v.col[1]/this->dt;
                step[order2].s.col[2] = step[order2].loc.pos.eci.v.col[2]/this->dt;
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
                step[order2].ss.col[0] = step[order2].loc.pos.eci.s.col[0]/this->dtsq;
                step[order2].ss.col[1] = step[order2].loc.pos.eci.s.col[1]/this->dtsq;
                step[order2].ss.col[2] = step[order2].loc.pos.eci.s.col[2]/this->dtsq;
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
                        step[order2+i*n].loc.pos.eci.v.col[0] = this->dt * (step[order2+i*n].s.col[0] + step[order2+i*n].sb.col[0]);
                        step[order2+i*n].loc.pos.eci.v.col[1] = this->dt * (step[order2+i*n].s.col[1] + step[order2+i*n].sb.col[1]);
                        step[order2+i*n].loc.pos.eci.v.col[2] = this->dt * (step[order2+i*n].s.col[2] + step[order2+i*n].sb.col[2]);
                        step[order2+i*n].loc.pos.eci.s.col[0] = this->dtsq * (step[order2+i*n].ss.col[0] + step[order2+i*n].sa.col[0]);
                        step[order2+i*n].loc.pos.eci.s.col[1] = this->dtsq * (step[order2+i*n].ss.col[1] + step[order2+i*n].sa.col[1]);
                        step[order2+i*n].loc.pos.eci.s.col[2] = this->dtsq * (step[order2+i*n].ss.col[2] + step[order2+i*n].sa.col[2]);

                        // Perform conversions between different systems
                        step[order2+i*n].loc.pos.eci.pass++;
                        pos_eci(&step[order2+i*n].loc);
                        att_icrf2lvlh(&step[order2+i*n].loc);
                        //		eci2earth(&step[order2+i*n].loc.pos,&step[order2+i*n].att);

                        // Calculate acceleration at new position
                        PosAccel(step[order2+i*n].loc, *newphys);

                        // Compare acceleration at new position to previous iteration
                        if (fabs(oldsa.col[0]-step[order2+i*n].loc.pos.eci.a.col[0])>1e-14 || fabs(oldsa.col[1]-step[order2+i*n].loc.pos.eci.a.col[1])>1e-14 || fabs(oldsa.col[2]-step[order2+i*n].loc.pos.eci.a.col[2])>1e-14)
                            cflag = 1;
                    }
                }
                c_cnt++;
            } while (c_cnt<10 && cflag);

            *newloc = step[order].loc;
            ++newloc->pos.eci.pass;
            pos_eci(newloc);
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

    }

}
