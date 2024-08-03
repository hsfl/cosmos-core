#include "physicsclass.h"
#include "support/enumlib.h"

namespace Cosmos
{
namespace Physics
{

//! Calculate dynamic physical attributes
//! Calculate various derived physical quantities that change, like heat, power generation, torque and drag
//! \param loc Pointer to ::locstruc
//! \param phys Pointer to ::physstruc
//! \return Zero, or negative error.
int32_t PhysCalc(locstruc* loc, physicsstruc* phys)
{
    Vector unitv = Quaternion(loc->att.geoc.s).irotate(Vector(loc->pos.geoc.v).normalize());
    Vector units = Quaternion(loc->att.icrf.s).irotate(Vector(loc->pos.icrf.s).normalize());
    Vector unite = Quaternion(loc->att.icrf.s).irotate(Vector(loc->pos.eci.s).normalize(-1.));
    Vector geov(loc->pos.geoc.v);
    double speed = geov.norm();
    double density;
    if (loc->pos.geod.s.h < 10000. || std::isnan(loc->pos.geod.s.h))
        density = 1.225;
    else
        density = 1000. * Msis00Density(loc->pos, 150., 150., 3.);
    double adrag = density * 1.1 * speed * speed;

    // External panel effects
    phys->adrag.clear();
    phys->atorque.clear();
    phys->rdrag.clear();
    phys->rtorque.clear();
    double surftemp = 310. - 80. * sin(loc->pos.geod.s.lat);
    double earthradiance = SIGMA * pow(surftemp,4);
    for (trianglestruc& triangle : phys->triangles)
    {
        triangle.sirradiation = 0.;
        triangle.eirradiation = 0.;
        if (triangle.external)
        {
            // Atmospheric effects
            double vdot = unitv.dot(triangle.normal);
            double sdot = units.dot((triangle.external==1?1.:-1.)*triangle.normal);
            if (vdot > 0. && phys->mass > 0.F)
            {
                double ddrag = adrag * vdot / phys->mass;
                Vector dtorque = ddrag * triangle.twist;
                phys->atorque += dtorque;
                Vector da = ddrag * triangle.shove;
                phys->adrag += da;
            }

            // Solar effects
            //                    double sirradiation;
            if (sdot > 0. && phys->mass > 0.F)
            {
                triangle.sirradiation = sdot * loc->pos.sunradiance;
                //                        triangle.sirradiation = triangle.pcell * sirradiation;
                double ddrag = triangle.sirradiation / (3e8*phys->mass);
                Vector dtorque = ddrag * triangle.twist;
                phys->rtorque += dtorque;
                Vector da = ddrag * triangle.shove;
                phys->rdrag += da;
            }

            // Earth effects
            double edot;
            if (triangle.external == 1)
            {
                edot = acos(unite.dot(triangle.normal) / triangle.normal.norm()) - RADOF(5.);
            }
            else
            {
                edot = acos(unite.dot(-triangle.normal) / triangle.normal.norm()) - RADOF(5.);
            }
            if (edot < 0.)
            {
                edot = 1.;
            }
            else
            {
                edot = cos(edot);
            }

            //                    double eirradiation = edot * SIGMA * pow(surftemp,4);
            //                    if (eirradiation > 0)
            if (edot > 0)
            {
                //                        triangle.eirradiation += triangle.area * triangle.pcell * eirradiation;
                triangle.eirradiation = triangle.area * edot * earthradiance;
            }

        }
    }
    return 0;
}

//! Calculate static physical attributes
//! Calculate various derived physical quantities, like Center of Mass and Moments of Inertia
//! \param loc Pointer to ::locstruc
//! \param phys Pointer to ::physstruc
//! \return Zero, or negative error.
int32_t PhysSetup(physicsstruc* phys)
{
    // Calculate Center of mass and internal area
    phys->area = 0.;
    phys->com.clear();
    phys->mass = 0.;
    for (trianglestruc triangle : phys->triangles)
    {
        if (triangle.external <= 1)
        {
            phys->area += triangle.area;
        }
        if (!triangle.external)
        {
            phys->area += triangle.area;
        }
        phys->com += triangle.com * triangle.mass;
        phys->mass += triangle.mass;
        phys->hcap += triangle.hcap;
    }
    if (phys->mass == 0.F)
    {
        return GENERAL_ERROR_TOO_LOW;
    }
    phys->com /= phys->mass;
    phys->hcap /= phys->mass;

    // Calculate Principal Moments of Inertia WRT COM
    phys->moi.clear();
    for (trianglestruc triangle : phys->triangles)
    {
        phys->moi.x += (phys->com.x - triangle.com.x) * (phys->com.x - triangle.com.x) * triangle.mass;
        phys->moi.y += (phys->com.y - triangle.com.y) * (phys->com.y - triangle.com.y) * triangle.mass;
        phys->moi.z += (phys->com.z - triangle.com.z) * (phys->com.z - triangle.com.z) * triangle.mass;
    }

    return 0;
}

//! Attitude acceleration
/*! Calculate the torque forces on the specified satellite at the specified location/
            \param physics Pointer to structure specifying satellite.
            \param loc Structure specifying location.
        */
int32_t AttAccel(locstruc &loc, physicsstruc &phys)
{
    return AttAccel(&loc, &phys);
}

int32_t AttAccel(locstruc *loc, physicsstruc *phys)
{
    //    rvector ue, ta, tv;
    //    rvector ttorque;
    Vector ue, ta, tv;
    Vector ttorque;
    rmatrix mom;

    att_extra(loc);

    ttorque = phys->ctorque;

    // Now calculate Gravity Gradient Torque
    // Unit vector towards earth, rotated into body frame
    //    ue = irotate((loc->att.icrf.s),rv_smult(-1.,loc->pos.eci.s));
    //    normalize_rv(ue);
    ue = Quaternion(loc->att.icrf.s).irotate(-1. * Vector(loc->pos.eci.s)).normalize();

    //    phys->gtorque = rv_smult((3.*GM/pow(loc->pos.geos.s.r,3.)),rv_cross(ue,rv_mult(phys->moi,ue)));
    phys->gtorque = (3. * GM / pow(loc->pos.geos.s.r,3.)) * ue.cross(phys->moi * ue);

    //    ttorque = rv_add(ttorque,phys->gtorque);
    ttorque += phys->gtorque;

    // Atmospheric and solar torque
    //	ttorque = rv_add(ttorque,phys->atorque);
    //	ttorque = rv_add(ttorque,phys->rtorque);

    // Torque from rotational effects

    // Moment of Inertia in Body frame
    mom = rm_diag(phys->moi.to_rv());
    // Attitude rate in Body frame
    tv = irotate(loc->att.icrf.s,loc->att.icrf.v);

    // Torque from cross product of angular velocity and angular momentum
    //    phys->htorque = rv_smult(-1., rv_cross(tv,rv_add(rv_mmult(mom,tv),phys->hmomentum)));
    //    ttorque = rv_add(ttorque,phys->htorque);
    phys->htorque = -1. * tv.cross(Vector(rv_mmult(mom, tv.to_rv())) + phys->hmomentum);
    ttorque += phys->htorque;

    // Convert torque into accelerations, doing math in Body frame

    // I x alpha = tau, so alpha = I inverse x tau
    //    ta = rv_mmult(rm_inverse(mom),ttorque);
    ta = Vector(rv_mmult(rm_inverse(mom),ttorque.to_rv()));

    // Convert body frame acceleration back to other frames.
    loc->att.icrf.a = irotate(q_conjugate(loc->att.icrf.s), ta.to_rv());
    loc->att.topo.a = irotate(q_conjugate(loc->att.topo.s), ta.to_rv());
    loc->att.lvlh.a = irotate(q_conjugate(loc->att.lvlh.s), ta.to_rv());
    loc->att.geoc.a = irotate(q_conjugate(loc->att.geoc.s), ta.to_rv());
    loc->att.selc.a = irotate(q_conjugate(loc->att.selc.s), ta.to_rv());
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
    return PosAccel(&loc, &phys);
}

int32_t PosAccel(locstruc* loc, physicsstruc* phys)
{
    int32_t iretn = 0;
    double radius;
    Vector ctpos, da, tda;
    cartpos bodypos;

    radius = length_rv(loc->pos.eci.s);

    loc->pos.eci.a = rv_zero();
    pos_eci2geoc(*loc);

    // Earth gravity
    // Calculate Geocentric acceleration vector

    if (radius > REARTHM)
    {
        // Start with gravity vector in ITRS

        da = GravityAccel(loc->pos,GravityEGM2008_NORM,12);

        // Correct for earth rotation, polar motion, precession, nutation

        da = Matrix(loc->pos.extra.e2j) * da;
    }
    else
    {
        // Simple 2 body
        da = -GM/(radius*radius*radius) * Vector(loc->pos.eci.s);
    }
    loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

    // Sun gravity
    // Calculate Satellite to Sun vector
    ctpos = rv_sub(rv_smult(-1., loc->pos.extra.sun2earth.s), loc->pos.eci.s);
    radius = ctpos.norm();
    da = GSUN/(radius*radius*radius) * ctpos;
    loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

    // Adjust for acceleration of frame
    radius = length_rv(loc->pos.extra.sun2earth.s);
    da = rv_smult(GSUN/(radius*radius*radius), loc->pos.extra.sun2earth.s);
    tda = da;
    loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

    // Moon gravity
    // Calculate Satellite to Moon vector
    bodypos.s = rv_sub( loc->pos.extra.sun2earth.s, loc->pos.extra.sun2moon.s);
    ctpos = rv_sub(bodypos.s, loc->pos.eci.s);
    radius = ctpos.norm();
    da = GMOON/(radius*radius*radius) * ctpos;
    loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

    // Adjust for acceleration of frame due to moon
    radius = length_rv(bodypos.s);
    da = rv_smult(GMOON/(radius*radius*radius),bodypos.s);
    tda -= da;
    loc->pos.eci.a = rv_sub(loc->pos.eci.a, da.to_rv());

    // Add thrust
    //            loc->pos.eci.a = rv_add(loc->pos.eci.a, rv_smult(1./phys->mass, phys->fpush.to_rv()));

    /*
        // Jupiter gravity
        // Calculate Satellite to Jupiter vector
        jplpos(JPL_EARTH,JPL_JUPITER, loc->pos.extra.tt,(cartpos *)&bodypos);
        ctpos = rv_sub(bodypos.s, loc->pos.eci.s);
        radius = length_rv(ctpos);

        // Calculate acceleration
        da = rv_smult(GJUPITER/(radius*radius*radius),ctpos);
        // loc->pos.eci.a = rv_add( loc->pos.eci.a,da);
        */


    Quaternion iratt = Quaternion(loc->att.icrf.s).conjugate();
    if (phys->adrag.norm() > 0.)
    {
        da = iratt.irotate(phys->adrag);
        loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
    }
    // Solar drag
    if (phys->rdrag.norm() > 0.)
    {
        da = iratt.irotate(phys->rdrag);
        loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
    }
    // Fictitious drag
    if (phys->fdrag.norm() > 0.)
    {
        da = iratt.irotate(phys->fdrag);
        loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
    }
    // Thrust
    if (phys->thrust.norm() > 0.)
    {
        da = iratt.irotate(phys->thrust);
        loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
    }
    // Fictional force
    //    if (phys->fpush.norm() > 0.)
    //    {
    //        da = (1. / phys->mass) * phys->fpush;
    //        loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
    //    }

    loc->pos.eci.pass++;
    iretn = pos_eci(loc);
    if (iretn < 0)
    {
        return iretn;
    }
    if (std::isnan( loc->pos.eci.a.col[0]))
    {
        loc->pos.eci.a.col[0] = 0.;
    }
    if (std::isnan( loc->pos.eci.a.col[1]))
    {
        loc->pos.eci.a.col[1] = 0.;
    }
    if (std::isnan( loc->pos.eci.a.col[2]))
    {
        loc->pos.eci.a.col[2] = 0.;
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

double Nplgndr(uint32_t l, uint32_t m, double x)
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

double Rearth(double lat)
{
    double st,ct;
    double c;

    st = sin(lat);
    ct = cos(lat);
    c = sqrt(((FRATIO2 * FRATIO2 * st * st) + (ct * ct))/((ct * ct) + (FRATIO2 * st * st)));
    return (REARTHM * c);
}

locstruc shape2eci(double utc, double altitude, double angle, double timeshift)
{
    return shape2eci(utc, 0., 0., altitude, angle, timeshift);
}

locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift)
{
    locstruc loc;

    //            longitude += 2. * DPI * (fabs(hour)/24. - (utc - (int)utc));

    pos_clear(loc);

    // Determine effects of oblate spheroid
    double ct = cos(latitude);
    double st = sin(latitude);
    double c = 1./sqrt(ct * ct + FRATIO2 * st * st);
    double s = FRATIO2 * c;
    double r = (Rearth(0.) * c + altitude) * ct;
    double z = ((Rearth(0.) * s + altitude) * st);
    double radius = sqrt(r * r + z * z);
    double phi = asin(z / radius);

    // Adjust for problems
    if (phi > angle)
    {
        phi = angle;
    }

    // Initial position
    Vector s0(radius, 0., 0.);
    double velocity = sqrt(GM/radius) - cos(angle) * radius * D2PI / 86400.;
    Vector v0(0., velocity, 0.);

    // First, rotate around X vector by angle
    Quaternion q1 = drotate_around_x(angle);
    Vector s1 = q1.drotate(s0);
    Vector v1 = q1.drotate(v0);

    double angle2;
    if (angle)
    {
        angle2 = asin(sin(phi) / sin(angle));
    }
    else
    {
        angle2 = 0.;
    }

    // Second, rotate around L vector by angle2, determine change imposed on longitude, then adjust for timeshift
    Vector L = (s1).cross(v1);
    Quaternion q2 = drotate_around(L, angle2);
    Vector s2 = q2.drotate(s1);
    double deltal = atan2(s2.y, s2.x);

    //            q2 = drotate_around(L, angle2 + 0.960 * timeshift * sqrt(GM/pow(radius,3.)));
    q2 = drotate_around(L, angle2 + timeshift * sqrt(GM/pow(radius,3.)));
    s2 = q2.drotate(s1);
    Vector v2 = q2.drotate(v1);


    // Third, rotate around Z vector by remaining distance, related to timeshift, longitude, and  change incurred from angle2)
    //            Quaternion q3 = drotate_around_z(-0.00 * timeshift*(D2PI/86400.) + longitude - deltal);
    Quaternion q3 = drotate_around_z(longitude - deltal);
    Vector s3 = q3.drotate(s2);
    Vector v3 = q3.drotate(v2);

    loc.pos.geoc.utc = loc.att.geoc.utc = utc;
    loc.pos.geoc.s.col[0] = s3.x;
    loc.pos.geoc.v.col[0] = v3.x;
    loc.pos.geoc.s.col[1] = s3.y;
    loc.pos.geoc.v.col[1] = v3.y;
    loc.pos.geoc.s.col[2] = s3.z;
    loc.pos.geoc.v.col[2] = v3.z;
    loc.pos.geoc.pass++;
    pos_geoc(loc);

    return loc;
}

//! Populate locstruc from file
//! Read JSON in provided file and populate locstruc based on contents:
//! - "type":"phys" : Physical orbit using ::Physics::shape2eci
//! - "type":"eci" : Orbit expressed in ECI coordinates
//! - "type":"kep" : Orbit expressed in Keplerian Elements
//! - "type":"tle" : Orbit expressed as TLE
//! \param fname File name for orbit
//! \param loc ::Convert::locstruc in to which to place orbit
//! \return Zero or negative error
int32_t load_loc(string fname, locstruc& loc)
{
    FILE *fdes;
    int32_t iretn;

    if ((fdes = fopen(fname.c_str(), "r")) == nullptr)
    {
        return (-errno);
    }

    char* input = nullptr;
    size_t n = 0;
    if ((iretn=getline(&input, &n, fdes)) < 0)
    {
        return -errno;
    }

    string estring;
    json11::Json jargs = json11::Json::parse(input, estring);
    free(input);
    if (!jargs["phys"].is_null())
    {
        json11::Json::object values = jargs["phys"].object_items();
        loc.pos.geod.s.lat = RADOF(values["lat"].number_value());
        loc.pos.geod.s.lon = RADOF(values["lon"].number_value());
        loc.pos.geod.s.h = values["alt"].number_value();
        double angle = RADOF(values["angle"].number_value());
        loc.pos.geod.utc = values["utc"].number_value();
        if (loc.pos.geod.utc == 0.)
        {
            loc.pos.geod.utc = currentmjd();
        }
        loc = Physics::shape2eci(loc.pos.geod.utc, loc.pos.geod.s.lat, loc.pos.geod.s.lon, loc.pos.geod.s.h,angle, 0.);
        return 0;
    }
    if (!jargs["eci"].is_null())
    {
        json11::Json::object values = jargs["eci"].object_items();
        loc.pos.eci.utc = (values["utc"].number_value());
        if (loc.pos.eci.utc == 0.)
        {
            loc.pos.eci.utc = currentmjd();
        }
        loc.pos.eci.s.col[0] = (values["x"].number_value());
        loc.pos.eci.s.col[1] = (values["y"].number_value());
        loc.pos.eci.s.col[2] = (values["z"].number_value());
        loc.pos.eci.v.col[0] = (values["vx"].number_value());
        loc.pos.eci.v.col[1] = (values["vy"].number_value());
        loc.pos.eci.v.col[2] = (values["vz"].number_value());
        loc.pos.eci.pass++;
        pos_eci(loc);
        return 0;
    }
    if (!jargs["kep"].is_null())
    {
        json11::Json::object values = jargs["kep"].object_items();
        kepstruc kep;
        kep.utc = (values["utc"].number_value());
        if (kep.utc == 0.)
        {
            kep.utc = currentmjd();
        }
        kep.ea = values["ea"].number_value();
        kep.i = values["i"].number_value();
        kep.ap = values["ap"].number_value();
        kep.raan = values["raan"].number_value();
        kep.e = values["e"].number_value();
        kep.a = values["a"].number_value();
        kep2eci(kep, loc.pos.eci);
        loc.pos.eci.pass++;
        pos_eci(loc);
        return 0;
    }
    if (!jargs["tle"].is_null())
    {
        json11::Json::object values = jargs["tle"].object_items();
        string fname = values["filename"].string_value();
        if ((iretn=load_tle(fname, loc.tle)) < 0)
        {
            return iretn;
        }
        tle2eci(loc.tle.utc, loc.tle, loc.pos.eci);
        loc.pos.eci.pass++;
        pos_eci(loc);
        return 0;
    }
    return GENERAL_ERROR_ARGS;
}

Vector ControlTorque(qatt tatt, qatt catt, Vector moi, double seconds)
{
    Matrix mom, rm;
    double dalp, domg, dt;
    Vector alpha, omega, torque;
    Vector distance;
    Quaternion dsq2;
    Quaternion tpos(tatt.s);
    Vector tvel(tatt.v);
    Quaternion cpos(catt.s);
    Vector cvel(catt.v);

    if ((dt=(tatt.utc-catt.utc)*86400.) > 0.) {

        // Calculate adjustment for ICRF distance

        //		if (length_q(tpos))
        if (tpos.norm()) {
            dsq2 = Quaternion(tpos) * Quaternion(cpos).conjugate();
            //			q_mult(tatt.s,q_conjugate(catt.s));
            distance.x = dsq2.x;
            distance.y = dsq2.y;
            distance.z = dsq2.z;
            //			rv_quaternion2axis(dsq2);
            domg = distance.norm();
            //			length_rv(distance);
            if (domg > DPI) {
                dsq2 *= -1.;
                //				= q_smult(-1.,dsq2);
                distance *= -1.;
                //				= rv_quaternion2axis(dsq2);
            }
            omega = distance / seconds;
            //			rv_smult(1./lag,distance);
        }
        else
        {
            // If Target quaternion is zero, adjust only for velocity
            omega = Vector();
            //			rv_zero();
        }

        // Match velocity
        omega += Vector(tvel);
        //		= rv_add(omega,tatt.v);
        alpha = omega - Vector(cvel);
        //		rv_sub(omega,catt.v);
        dalp = alpha.norm() / seconds;
        //		length_rv(alpha) / lag;
        alpha = dalp * alpha.normalize();
        //		rv_smult(dalp,rv_normal(alpha));

        // Conversion from ICRF to Body
        rm = Matrix(Quaternion(cpos));
        //		rm_quaternion2dcm(catt.s);
        // Moment of Inertia in Body
        mom = mom.diag(moi);
        //		rm_diag(moi);
        // Moment of Inertia in ICRF
        mom = rm.transpose() * (mom * rm);
        //		rm_mmult(rm_transloc.pos.eci.se(rm),rm_mmult(mom,rm));
        torque = mom * alpha;
        //		rv_mmult(mom,alpha);
    }
    return torque;
}

Vector ControlThrust(cartpos current, cartpos goal, double mass, double maxaccel, double seconds)
{
    return mass * ControlAccel(current, goal, maxaccel, seconds);
}

Vector ControlAccel(cartpos current, cartpos goal, double maxaccel, double seconds)
{
    double minaccel = 0.1 * maxaccel;
    Vector newa;

    Vector dpos = Vector(rv_sub(current.s, goal.s));
    Vector dvel = Vector(rv_sub(current.v, goal.v));

    if (dpos.norm() > .01)
    {
        Vector timet;
        double s0;
        double v0;
        double vt;
        //        double v2;
        //        double sa2;
        //        double tsp;
        //        double tsm;
        //        double tv;

        // X
        s0 = dpos.x;
        v0 = dvel.x;
        //        v2 = v0 * v0;
        if (s0 > 0)
        {
            vt = -sqrt(2.0 * maxaccel * s0);
            newa.x = (vt - v0) / seconds;
        }
        else if (s0 < 0)
        {
            vt = sqrt(2.0 * maxaccel * -s0);
            newa.x = (vt - v0) / seconds;
        }

        // Y
        s0 = dpos.y;
        v0 = dvel.y;
        if (s0 > 0)
        {
            vt = -sqrt(2.0 * maxaccel * s0);
            newa.y = (vt - v0) / seconds;
        }
        else if (s0 < 0)
        {
            vt = sqrt(2.0 * maxaccel * -s0);
            newa.y = (vt - v0) / seconds;
        }

        // Z
        s0 = dpos.z;
        v0 = dvel.z;
        if (s0 > 0)
        {
            vt = -sqrt(2.0 * maxaccel * s0);
            newa.z = (vt - v0) / seconds;
        }
        else if (s0 < 0)
        {
            vt = sqrt(2.0 * maxaccel * -s0);
            newa.z = (vt - v0) / seconds;
        }

        newa = newa.maxmag(maxaccel).minmag(minaccel);
    }
//    newa.clear();
    return newa;
}

int32_t Structure::Setup(string stype)
{
    static Enum stypes;
    if (!stypes.Size())
    {
        stypes.Init({
                        "NoType",
                        "U1","U1X","U1Y","U1XY",
                        "U1_5","U1_5X","U1_5Y","U1_5XY",
                        "U2","U2X","U2Y","U2XY",
                        "U3","U3X","U3Y","U3XY",
                        "U6","U6X","U6Y","U6XY",
                        "U12","U12X","U12Y","U12XY",
                        "HEX65W80H"
                    }, {0});
    }

    if (stypes.Exists(stype))
    {
        return Setup(static_cast<Type>(stypes[stype]));
    }
    else
    {
        return Type::NoType;
    }
}

int32_t Structure::Setup(Type type)
{
    int32_t iretn = 0;

    switch (type)
    {
    case NoType:
        iretn = add_u();
    case U1:
        iretn = add_u(1, 1, 1, NoPanel);
        break;
    case U1X:
        iretn = add_u(1, 1, 1, X);
        break;
    case U1Y:
        iretn = add_u(1, 1, 1, Y);
        break;
    case U1XY:
        iretn = add_u(1, 1, 1, XY);
        break;
    case U1_5:
        iretn = add_u(1, 1, 1.5, NoPanel);
        break;
    case U1_5X:
        iretn = add_u(1, 1, 1.5, X);
        break;
    case U1_5Y:
        iretn = add_u(1, 1, 1.5, Y);
        break;
    case U1_5XY:
        iretn = add_u(1, 1, 1.5, XY);
        break;
    case U3:
        iretn = add_u(1, 1, 3, NoPanel);
        break;
    case U3X:
        iretn = add_u(1, 1, 3, X);
        break;
    case U3Y:
        iretn = add_u(1, 1, 3, Y);
        break;
    case U3XY:
        iretn = add_u(1, 1, 3, XY);
        break;
    case U6:
        iretn = add_u(1, 2, 3, NoPanel);
        break;
    case U6X:
        iretn = add_u(1, 2, 3, X);
        break;
    case U6Y:
        iretn = add_u(1, 2, 3, Y);
        break;
    case U6XY:
        iretn = add_u(1, 2, 3, XY);
        break;
    case U12:
        iretn = add_u(2, 2, 3, NoPanel);
        break;
    case U12X:
        iretn = add_u(2, 2, 3, X);
        break;
    case U12Y:
        iretn = add_u(2, 2, 3, Y);
        break;
    case U12XY:
        iretn = add_u(2, 2, 3, XY);
        break;
    case HEX65W80H:
        iretn = add_hex(.65, .80, NoPanel);
        break;
    default:
        iretn =  GENERAL_ERROR_OUTOFRANGE;
    }

    if (iretn < 0)
    {
        return iretn;
    }

    // Calculate physical quantities
    iretn = PhysSetup(currentphys);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t Structure::add_u(double x, double y, double z, ExternalPanelType type)
{
    x *= .1;
    y *= .1;
    z *= .1;
    switch (type)
    {
    case NoType:
        add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01);
        add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01);
        add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01);
        add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01);
        add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
        add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
        break;
    case X:
        add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01, 2, 0.);
        add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01, 2, 0.);
        add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .01);
        add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .01);

        add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01);
        add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01);

        add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
        add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
        break;
    case Y:
        add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01);
        add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01);

        add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01, 2, 0.);
        add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01, 2, 0.);
        add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .01);
        add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .01);

        add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
        add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
        break;
    case XY:
        add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01, 2, 0.);
        add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01, 2, 0.);
        add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .01);
        add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .01);

        add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(-x/2., y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01, 2, 0.);
        add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01, 2, 0.);
        add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .01);
        add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .01);

        add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
        add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
        break;
    }

    return 0;
}

int32_t Structure::add_hex(double width, double height, ExternalPanelType type)
{
    switch (type)
    {
    case NoType:
        for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
        }
        break;
    case X:
        for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
        }
        break;
    case Y:
        for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
        }
        break;
    case XY:
        for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
        }
        break;
    }

    return 0;
}

int32_t Structure::add_oct(double width, double height, ExternalPanelType type)
{
    switch (type)
    {
    case NoType:
        for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
        }

        break;
    case X:
        for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
        }

        break;
    case Y:
        for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
        }

        break;
    case XY:
        for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
        {
            add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
            add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
            add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
        }

        break;
    }

    return 0;
}

int32_t Structure::add_cuboid(string name, Vector size, double depth, Quaternion orientation, Vector offset)
{
    add_face(name+"+x", Vector(size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
    add_face(name+"-x", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., -size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);
    add_face(name+"+y", Vector(-size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
    add_face(name+"-y", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);
    add_face(name+"+z", Vector(-size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
    add_face(name+"-z", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., -size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);

    return 0;
}

int32_t Structure::add_face(string name, Vector point0, Vector point1, Vector point2, Vector point3, double depth, uint8_t external, float pcell, Quaternion orientation, Vector offset)
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
    triangle.com = (currentphys->vertices[triangle.tidx[0]] + currentphys->vertices[triangle.tidx[1]] + currentphys->vertices[triangle.tidx[2]]) / 3.;
    triangle.area = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).area(currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[0]]);
    triangle.normal = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).cross(currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[0]]).normalize(triangle.area);
    triangle.mass = triangle.area * triangle.depth * triangle.density;
    triangle.perimeter = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).norm() + (currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[1]]).norm() + (currentphys->vertices[triangle.tidx[0]] - currentphys->vertices[triangle.tidx[2]]).norm();

    triangle.twist = (triangle.com - triangle.normal).cross(triangle.normal);
    triangle.twist = (-triangle.area/(triangle.normal.norm2()) * triangle.twist);
    triangle.shove = Vector();
    for (uint16_t i=0; i<2; i++)
    {
        Vector tv0 = (currentphys->vertices[triangle.tidx[i]] - triangle.com);
        Vector tv1 = (currentphys->vertices[triangle.tidx[i+1]] - triangle.com);
        double ta = (tv0.norm());
        for (uint16_t j=0; j<=ta*100; j++)
        {
            Vector tv2 = (tv0 * .01*j/ta);
            Vector tv3 = (tv1 * .01*j/ta);
            Vector dv = (tv3 - tv2);
            double tb = (dv.norm());
            for (size_t k=0; k<tb*100; k++)
            {
                Vector sv = (triangle.com + (tv2 + (dv * .01*k/tb)));
                double tc = 1./(sv.norm2());
                triangle.shove = (triangle.shove + (sv * tc));
            }
        }
    }
    triangle.shove = (triangle.shove / -10000.);

    currentphys->triangles.push_back(triangle);

    return 1;
}

int32_t Structure::add_vertex(Vector point)
{
    bool found = false;

    int32_t index = -1;
    for (uint16_t i=0; i<currentphys->vertices.size(); ++ i)
    {
        if ((point - currentphys->vertices[i]).norm() < .001)
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
        currentphys->vertices.push_back(point);
        return currentphys->vertices.size() - 1;
    }
}


int32_t State::Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, tlestruc tle, double utc, qatt icrf)
{
    dt = 86400.*((currentinfo.node.loc.utc + (idt / 86400.))-currentinfo.node.loc.utc);
    dtj = dt / 86400.;

    currentinfo.node.name = name;
    currentinfo.agent0.name = "sim";
    currentinfo.node.loc.utc = utc;
    tle2eci(currentinfo.node.loc.utc, tle, currentinfo.node.loc.pos.eci);
    currentinfo.node.loc.tle = tle;
    currentinfo.node.loc.pos.eci.pass++;
    pos_eci(currentinfo.node.loc);

    structure = new Structure(&currentinfo.node.phys);
    structure->Setup(stype);
    this->stype = stype;

    switch (ptype)
    {
    case Propagator::Type::PositionInertial:
        inposition = new InertialPositionPropagator(&currentinfo, dt);
        dt = inposition->dt;
        dtj = inposition->dtj;
        break;
    case Propagator::Type::PositionIterative:
        itposition = new IterativePositionPropagator(&currentinfo, dt);
        dt = itposition->dt;
        dtj = itposition->dtj;
        break;
    case Propagator::Type::PositionGaussJackson:
        gjposition = new GaussJacksonPositionPropagator(&currentinfo, dt, 6);
        dt = gjposition->dt;
        dtj = gjposition->dtj;
        gjposition->Init();
        break;
    case Propagator::Type::PositionGeo:
        geoposition = new GeoPositionPropagator(&currentinfo, dt);
        dt = geoposition->dt;
        dtj = geoposition->dtj;
        break;
    case Propagator::Type::PositionTle:
        tleposition = new TlePositionPropagator(&currentinfo, dt);
        dt = tleposition->dt;
        dtj = tleposition->dtj;
        break;
    case Propagator::Type::PositionLvlh:
        lvlhposition = new LvlhPositionPropagator(&currentinfo, dt);
        dt = lvlhposition->dt;
        dtj = lvlhposition->dtj;
        break;
    default:
        inposition = new InertialPositionPropagator(&currentinfo, dt);
        dt = inposition->dt;
        dtj = inposition->dtj;
        break;
    }
    this->ptype = ptype;

    switch (atype)
    {
    case Propagator::Type::AttitudeInertial:
        inattitude = new InertialAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.icrf.pass++;
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
        break;
    case Propagator::Type::AttitudeIterative:
        itattitude = new IterativeAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.icrf.pass++;
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
        break;
    case Propagator::Type::AttitudeLVLH:
        lvattitude = new LvlhAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.lvlh.s = q_eye();
        currentinfo.node.loc.att.lvlh.v = rv_zero();
        currentinfo.node.loc.att.lvlh.a = rv_zero();
        currentinfo.node.loc.att.lvlh.utc = utc;
        currentinfo.node.loc.att.lvlh.pass++;
        att_lvlh(currentinfo.node.loc);
        break;
    case Propagator::Type::AttitudeGeo:
        geoattitude = new GeoAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.geoc.pass++;
        att_geoc(currentinfo.node.loc);
        break;
    case Propagator::Type::AttitudeSolar:
        solarattitude = new SolarAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.icrf.pass++;
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
        break;
    case Propagator::Type::AttitudeTarget:
    {
        targetattitude = new TargetAttitudePropagator(&currentinfo, dt);
        lvattitude = new LvlhAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.lvlh.s = q_eye();
        currentinfo.node.loc.att.lvlh.v = rv_zero();
        currentinfo.node.loc.att.lvlh.a = rv_zero();
        currentinfo.node.loc.att.lvlh.utc = utc;
        currentinfo.node.loc.att.lvlh.pass++;
        att_lvlh(currentinfo.node.loc);
        break;
    }
    default:
    {
        inattitude = new InertialAttitudePropagator(&currentinfo, dt);
        currentinfo.node.loc.att.icrf.pass++;
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
        break;
    }
    }
    this->atype = atype;

    switch (ttype)
    {
    case Propagator::Type::Thermal:
        thermal = new ThermalPropagator(&currentinfo, dt, 300.);
        break;
    default:
        thermal = new ThermalPropagator(&currentinfo, dt, 300.);
        break;
    }
    this->ttype = ttype;

    switch (etype)
    {
    case Propagator::Type::Electrical:
        electrical = new ElectricalPropagator(&currentinfo, dt, .5);
        break;
    default:
        electrical = new ElectricalPropagator(&currentinfo, dt, .5);
        break;
    }
    this->etype = etype;

    if (ptype == Propagator::PositionGeo)
    {
        currentinfo.node.loc.pos.geod.pass++;
        pos_geod(currentinfo.node.loc);
    }
    else
    {
        currentinfo.node.loc.pos.eci.pass++;
        pos_eci(currentinfo.node.loc);
        PosAccel(currentinfo.node.loc, currentinfo.node.phys);
    }

    orbitalevent = new OrbitalEventGenerator(&currentinfo, dt);
    metric = new MetricGenerator(&currentinfo, dt);

    initialloc = currentinfo.node.loc;
    initialphys = currentinfo.node.phys;
    currentinfo.node.utc = currentinfo.node.loc.utc;
    return 0;
}

int32_t State::Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, cartpos eci, qatt icrf)
{
    int32_t iretn = 0;
    pos_clear(currentinfo.node.loc);
    currentinfo.node.loc.pos.eci = eci;
    currentinfo.node.loc.pos.eci.pass++;
    iretn = pos_eci(currentinfo.node.loc);
    if (iretn < 0)
    {
        return iretn;
    }
    if (length_q(icrf.s) <= __DBL_MIN__)
    {
        currentinfo.node.loc.att.lvlh.pass++;
        currentinfo.node.loc.att.lvlh.s = q_eye();
        currentinfo.node.loc.att.lvlh.v = rv_zero();
        currentinfo.node.loc.att.lvlh.a = rv_zero();
        currentinfo.node.loc.att.lvlh.utc = eci.utc;
        iretn = att_lvlh(currentinfo.node.loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    else
    {
        currentinfo.node.loc.att.icrf = icrf;
        currentinfo.node.loc.att.icrf.pass++;
        iretn = att_icrf(currentinfo.node.loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }

    return Init(name, idt, stype, ptype, atype, ttype, etype);
}

int32_t State::Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, cartpos eci, cartpos lvlh, qatt icrf)
{
    int32_t iretn = 0;
    pos_clear(currentinfo.node.loc);
    currentinfo.node.loc.pos.eci = eci;
    currentinfo.node.loc.pos.lvlh = lvlh;
    currentinfo.node.loc.pos.eci.pass++;
    iretn = pos_eci(currentinfo.node.loc);
    if (iretn < 0)
    {
        return iretn;
    }
    if (length_q(icrf.s) <= __DBL_MIN__)
    {
        currentinfo.node.loc.att.lvlh.pass++;
        currentinfo.node.loc.att.lvlh.s = q_eye();
        currentinfo.node.loc.att.lvlh.v = rv_zero();
        currentinfo.node.loc.att.lvlh.a = rv_zero();
        currentinfo.node.loc.att.lvlh.utc = eci.utc;
        iretn = att_lvlh(currentinfo.node.loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    else
    {
        currentinfo.node.loc.att.icrf = icrf;
        currentinfo.node.loc.att.icrf.pass++;
        iretn = att_icrf(currentinfo.node.loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    iretn = pos_origin2lvlh(currentinfo.node.loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return Init(name, idt, stype, ptype, atype, ttype, etype);
}

int32_t State::Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype)
{
    dt = 86400.*((currentinfo.node.loc.utc + (idt / 86400.))-currentinfo.node.loc.utc);
    dtj = dt / 86400.;

    currentinfo.node.name = name;
    currentinfo.agent0.name = "sim";

    structure = new Structure(&currentinfo.node.phys);
    structure->Setup(stype);
    this->stype = stype;
    currentinfo.mass = currentinfo.node.phys.mass;

    switch (ptype)
    {
    case Propagator::Type::PositionInertial:
        inposition = new InertialPositionPropagator(&currentinfo, dt);
        inposition->Init();
        break;
    case Propagator::Type::PositionIterative:
        itposition = new IterativePositionPropagator(&currentinfo, dt);
        itposition->Init();
        break;
    case Propagator::Type::PositionGaussJackson:
        gjposition = new GaussJacksonPositionPropagator(&currentinfo, dt, 6);
        gjposition->Init();
        break;
    case Propagator::Type::PositionGeo:
        geoposition = new GeoPositionPropagator(&currentinfo, dt);
        geoposition->Init();
        break;
    case Propagator::Type::PositionTle:
        tleposition = new TlePositionPropagator(&currentinfo, dt);
        tleposition->Init();
        break;
    case Propagator::Type::PositionLvlh:
        lvlhposition = new LvlhPositionPropagator(&currentinfo, dt);
        lvlhposition->Init();
        break;
    default:
        inposition = new InertialPositionPropagator(&currentinfo, dt);
        inposition->Init();
        break;
    }
    this->ptype = ptype;

    switch (atype)
    {
    case Propagator::Type::AttitudeInertial:
        inattitude = new InertialAttitudePropagator(&currentinfo, dt);
        inattitude->Init();
        break;
    case Propagator::Type::AttitudeGeo:
        geoattitude = new GeoAttitudePropagator(&currentinfo, dt);
        geoattitude->Init();
        break;
    case Propagator::Type::AttitudeSolar:
        solarattitude = new SolarAttitudePropagator(&currentinfo, dt);
        solarattitude->Init();
        break;
    case Propagator::Type::AttitudeIterative:
        itattitude = new IterativeAttitudePropagator(&currentinfo, dt);
        itattitude->Init();
        break;
    case Propagator::Type::AttitudeLVLH:
        lvattitude = new LvlhAttitudePropagator(&currentinfo, dt);
        lvattitude->Init();
        break;
    case Propagator::Type::AttitudeTarget:
    {
        targetattitude = new TargetAttitudePropagator(&currentinfo, dt);
        //        lvattitude = new LvlhAttitudePropagator(&currentinfo, dt);
        targetattitude->Init();
    }
    break;
    default:
        inattitude = new InertialAttitudePropagator(&currentinfo, dt);
        inattitude->Init();
        break;
    }
    this->atype = atype;

    switch (ttype)
    {
    case Propagator::Type::Thermal:
        thermal = new ThermalPropagator(&currentinfo, dt, 600.);
        thermal->Init(thermal->temperature);
        break;
    default:
        thermal = new ThermalPropagator(&currentinfo, dt, 300.);
        thermal->Init(thermal->temperature);
        break;
    }
    this->ttype = ttype;

    switch (etype)
    {
    case Propagator::Type::Electrical:
        electrical = new ElectricalPropagator(&currentinfo, dt, .5);
        electrical->Init();
        break;
    default:
        electrical = new ElectricalPropagator(&currentinfo, dt, .5);
        electrical->Init();
        break;
    }
    this->etype = etype;

    if (ptype == Propagator::PositionGeo)
    {
        pos_geod(currentinfo.node.loc);
    }
    else
    {
        pos_eci(currentinfo.node.loc);
        PosAccel(currentinfo.node.loc, currentinfo.node.phys);
    }
    if (atype == Propagator::AttitudeGeo)
    {
        att_geoc(currentinfo.node.loc);
    }
    else
    {
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
    }

    orbitalevent = new OrbitalEventGenerator(&currentinfo, dt);
    metric = new MetricGenerator(&currentinfo, dt);

    initialloc = currentinfo.node.loc;
    initialphys = currentinfo.node.phys;
    currentinfo.node.utc = currentinfo.node.loc.utc;
    return 0;
}

int32_t State::Propagate(double nextutc)
{
    int32_t count = 0;
    if (nextutc == 0.)
    {
        nextutc = currentinfo.node.utc + dtj;
    }

    while ((nextutc - currentinfo.node.utc) > dtj / 2.)
    {
        PhysCalc(&currentinfo.node.loc, &currentinfo.node.phys);

        // Thermal
        static_cast<ThermalPropagator *>(thermal)->Propagate(nextutc);

        // Electrical
        static_cast<ElectricalPropagator *>(electrical)->Propagate(nextutc);

        // Position
        switch (ptype)
        {
        case Propagator::Type::PositionIterative:
            static_cast<IterativePositionPropagator *>(itposition)->Propagate(nextutc);
            break;
        case Propagator::Type::PositionInertial:
            static_cast<InertialPositionPropagator *>(inposition)->Propagate(nextutc);
            break;
        case Propagator::Type::PositionGaussJackson:
            static_cast<GaussJacksonPositionPropagator *>(gjposition)->Propagate(nextutc, currentinfo.node.loc.att.icrf.s);
            break;
        case Propagator::Type::PositionGeo:
            static_cast<GeoPositionPropagator *>(geoposition)->Propagate(nextutc);
            break;
        case Propagator::Type::PositionTle:
            static_cast<TlePositionPropagator *>(tleposition)->Propagate(nextutc);
            break;
        case Propagator::Type::PositionLvlh:
            static_cast<LvlhPositionPropagator *>(lvlhposition)->Propagate(currentinfo.node.loc);
            break;
        default:
            break;
        }

        if (ptype == Propagator::PositionGeo)
        {
            pos_geod(currentinfo.node.loc);
        }
        else
        {
            pos_eci(currentinfo.node.loc);
        }
        if (atype == Propagator::AttitudeGeo)
        {
            att_geoc(currentinfo.node.loc);
        }
        else
        {
            att_icrf(currentinfo.node.loc);
            AttAccel(currentinfo.node.loc, currentinfo.node.phys);
        }

        update_target(&currentinfo);

        // Attitude
        switch (atype)
        {
        case Propagator::Type::AttitudeIterative:
            static_cast<IterativeAttitudePropagator *>(itattitude)->Propagate(nextutc);
            break;
        case Propagator::Type::AttitudeInertial:
            static_cast<InertialAttitudePropagator *>(inattitude)->Propagate(nextutc);
            break;
        case Propagator::Type::AttitudeLVLH:
            static_cast<LvlhAttitudePropagator *>(lvattitude)->Propagate(nextutc);
            break;
        case Propagator::Type::AttitudeGeo:
            static_cast<GeoAttitudePropagator *>(geoattitude)->Propagate(nextutc);
            break;
        case Propagator::Type::AttitudeSolar:
            static_cast<SolarAttitudePropagator *>(solarattitude)->Propagate(nextutc);
            break;
        case Propagator::Type::AttitudeTarget:
            // Update again at end after currentinfo->target update
            static_cast<TargetAttitudePropagator *>(targetattitude)->Propagate(nextutc);
            break;
        default:
            break;
        }

        // Orbital event propagator
        if (orbitalevent != nullptr)
        {
            orbitalevent->Propagate(nextutc);
        }

        // Metric propagator
        if (metric != nullptr)
        {
            metric->Propagate(nextutc);
        }

        // Update time
        currentinfo.node.utc += dtj;
        ++count;
    } ;
    return count;
}

int32_t State::Propagate(locstruc &loc)
{
    int32_t count = 0;
    double nextutc = loc.utc;
    PhysCalc(&currentinfo.node.loc, &currentinfo.node.phys);

    // Thermal
    static_cast<ThermalPropagator *>(thermal)->Propagate(nextutc);

    // Electrical
    static_cast<ElectricalPropagator *>(electrical)->Propagate(nextutc);

    // Position
    switch (ptype)
    {
    case Propagator::Type::PositionIterative:
        static_cast<IterativePositionPropagator *>(itposition)->Propagate(nextutc);
        break;
    case Propagator::Type::PositionInertial:
        static_cast<InertialPositionPropagator *>(inposition)->Propagate(nextutc);
        break;
    case Propagator::Type::PositionGaussJackson:
        static_cast<GaussJacksonPositionPropagator *>(gjposition)->Propagate(nextutc, currentinfo.node.loc.att.icrf.s);
        break;
    case Propagator::Type::PositionGeo:
        static_cast<GeoPositionPropagator *>(geoposition)->Propagate(nextutc);
        break;
    case Propagator::Type::PositionTle:
        static_cast<TlePositionPropagator *>(tleposition)->Propagate(nextutc);
        break;
    case Propagator::Type::PositionLvlh:
    {
        static_cast<LvlhPositionPropagator *>(lvlhposition)->Propagate(loc);
    }
    break;
    default:
        break;
    }

    if (ptype == Propagator::PositionGeo)
    {
        pos_geod(currentinfo.node.loc);
    }
    else
    {
        pos_eci(currentinfo.node.loc);
    }
    if (atype == Propagator::AttitudeGeo)
    {
        att_geoc(currentinfo.node.loc);
    }
    else
    {
        att_icrf(currentinfo.node.loc);
        AttAccel(currentinfo.node.loc, currentinfo.node.phys);
    }

    update_target(&currentinfo);

    // Attitude
    switch (atype)
    {
    case Propagator::Type::AttitudeIterative:
        static_cast<IterativeAttitudePropagator *>(itattitude)->Propagate(nextutc);
        break;
    case Propagator::Type::AttitudeInertial:
        static_cast<InertialAttitudePropagator *>(inattitude)->Propagate(nextutc);
        break;
    case Propagator::Type::AttitudeLVLH:
        static_cast<LvlhAttitudePropagator *>(lvattitude)->Propagate(nextutc);
        break;
    case Propagator::Type::AttitudeGeo:
        static_cast<GeoAttitudePropagator *>(geoattitude)->Propagate(nextutc);
        break;
    case Propagator::Type::AttitudeSolar:
        static_cast<SolarAttitudePropagator *>(solarattitude)->Propagate(nextutc);
        break;
    case Propagator::Type::AttitudeTarget:
        // Update again at end after currentinfo->target update
        static_cast<TargetAttitudePropagator *>(targetattitude)->Propagate(nextutc);
        break;
    default:
        break;
    }

    // Orbital event propagator
    if (orbitalevent != nullptr)
    {
        orbitalevent->Propagate(nextutc);
    }

    // Metric propagator
    if (metric != nullptr)
    {
        metric->Propagate(nextutc);
    }

    // Update time
    currentinfo.node.utc += dtj;
    ++count;
    return count;
}

int32_t State::End()
{
    // Orbital event propagator
    if (orbitalevent != nullptr)
    {
        orbitalevent->End();
    }

    return 0.;
}

int32_t State::Reset(double nextutc)
{
    int32_t iretn = 0;
    currentinfo.node.loc = initialloc;
    currentinfo.node.utc = currentinfo.node.loc.utc;
    iretn = Propagate(nextutc);

    return iretn;
}

//int32_t State::AddTarget(std::string name, locstruc loc, NODE_TYPE type, gvector size)
//{
//    targetstruc ttarget;
//    ttarget.type = type;
//    ttarget.name = name;
//    ttarget.cloc = loc;
//    ttarget.area = 0.;
//    ttarget.size = size;
//    ttarget.loc = loc;

//    currentinfo.target.push_back(ttarget);
//    return currentinfo.target.size();
//}

//int32_t State::AddTarget(std::string name, locstruc loc, NODE_TYPE type, double area)
//{
//    targetstruc ttarget;
//    ttarget.type = type;
//    ttarget.name = name;
//    ttarget.cloc = loc;
//    ttarget.size = gvector();
//    ttarget.area  = area;
//    ttarget.loc = loc;

//    currentinfo.target.push_back(ttarget);
//    return currentinfo.target.size();
//}

//int32_t State::AddTarget(string name, double lat, double lon, double alt, NODE_TYPE type)
//{
//    return AddTarget(name, lat, lon, DPI * 1e6, alt, type);
//}

//int32_t State::AddTarget(string name, double lat, double lon, double area, double alt, NODE_TYPE type)
//{
//    locstruc loc;
//    loc.pos.geod.pass = 1;
//    loc.pos.geod.utc = currentinfo.node.utc;
//    loc.pos.geod.s.lat = lat;
//    loc.pos.geod.s.lon = lon;
//    loc.pos.geod.s.h = alt;
//    loc.pos.geod.v = gv_zero();
//    loc.pos.geod.a = gv_zero();
//    loc.pos.geod.pass++;
//    pos_geod(loc);
//    return AddTarget(name, loc, type, area);
//}

//int32_t State::AddTarget(string name, double ullat, double ullon, double lrlat, double lrlon, double alt, NODE_TYPE type)
//{
//    locstruc loc;
//    loc.pos.geod.pass = 1;
//    loc.pos.geod.utc = currentinfo.node.utc;
//    loc.pos.geod.s.lat = (ullat + lrlat) / 2.;
//    loc.pos.geod.s.lon = (ullon + lrlon) / 2.;
//    loc.pos.geod.s.h = alt;
//    loc.pos.geod.v = gv_zero();
//    loc.pos.geod.a = gv_zero();
//    //            gvector size(ullat-lrlat, lrlon-ullon, 0.);
//    loc.pos.geod.pass++;
//    pos_geod(loc);
//    double area = (ullat-lrlat) * (cos(lrlon) * lrlon - cos(ullon) * ullon) * REARTHM * REARTHM;
//    return AddTarget(name, loc, type, area);
//}

int32_t InertialAttitudePropagator::Init()
{

    return  0;
}

int32_t InertialAttitudePropagator::Reset(double nextutc)
{
    currentinfo->node.loc.att = initialloc.att;
    currentutc = currentinfo->node.loc.att.utc;
    Propagate(nextutc);

    return  0;
}

int32_t InertialAttitudePropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentinfo->node.loc.att.icrf = initialloc.att.icrf;
    currentutc = nextutc;
    currentinfo->node.loc.att.icrf.utc = nextutc;
    currentinfo->node.loc.att.icrf.pass++;
    att_icrf(currentinfo->node.loc);

    return 0;
}

int32_t GeoAttitudePropagator::Init()
{

    return  0;
}

int32_t GeoAttitudePropagator::Reset(double nextutc)
{
    currentinfo->node.loc.att = initialloc.att;
    currentutc = currentinfo->node.loc.att.utc;
    Propagate(nextutc);

    return  0;
}

int32_t GeoAttitudePropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentinfo->node.loc.att.geoc = initialloc.att.geoc;
    currentutc = nextutc;
    currentinfo->node.loc.att.geoc.utc = nextutc;
    currentinfo->node.loc.att.geoc.pass++;
    att_geoc(currentinfo->node.loc);

    return 0;
}

int32_t IterativeAttitudePropagator::Init()
{
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);

    return  0;
}

int32_t IterativeAttitudePropagator::Reset(double nextutc)
{
    currentinfo->node.loc.att = initialloc.att;
    currentutc = currentinfo->node.loc.att.utc;
    Propagate(nextutc);

    return 0;
}

int32_t IterativeAttitudePropagator::Propagate(double nextutc)
{
    quaternion q1;

    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        q1 = q_axis2quaternion_rv(rv_smult(dt, currentinfo->node.loc.att.icrf.v));
        currentinfo->node.loc.att.icrf.s = q_fmult(q1, currentinfo->node.loc.att.icrf.s);
        normalize_q(&currentinfo->node.loc.att.icrf.s);
        // Calculate new v from da
        currentinfo->node.loc.att.icrf.v = rv_add(currentinfo->node.loc.att.icrf.v, rv_smult(dt, currentinfo->node.loc.att.icrf.a));
        currentinfo->node.loc.att.icrf.utc = currentutc;
        currentinfo->node.loc.att.icrf.pass++;
        AttAccel(currentinfo->node.loc, currentinfo->node.phys);
        att_icrf(currentinfo->node.loc);
    }

    return 0;
}

int32_t LvlhAttitudePropagator::Init()
{
    currentinfo->node.loc.att.lvlh.utc = currentutc;
    currentinfo->node.loc.att.lvlh.s = q_eye();
    currentinfo->node.loc.att.lvlh.v = rv_zero();
    currentinfo->node.loc.att.lvlh.a = rv_zero();
    ++currentinfo->node.loc.att.lvlh.pass;
    att_lvlh2icrf(currentinfo->node.loc);
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);
    att_icrf(currentinfo->node.loc);

    return  0;
}

int32_t LvlhAttitudePropagator::Reset(double nextutc)
{
    currentinfo->node.loc.att = initialloc.att;
    currentutc = currentinfo->node.loc.att.utc;
    Propagate(nextutc);

    return  0;
}

int32_t LvlhAttitudePropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }

    currentutc = nextutc;
    currentinfo->node.loc.att.lvlh.utc = nextutc;
    currentinfo->node.loc.att.lvlh.s = q_eye();
    currentinfo->node.loc.att.lvlh.v = rv_zero();
    currentinfo->node.loc.att.lvlh.a = rv_zero();
    ++currentinfo->node.loc.att.lvlh.pass;
    att_lvlh(currentinfo->node.loc);
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t SolarAttitudePropagator::Init()
{
    optimum = Quaternion(currentinfo->node.loc.att.icrf.s).drotate(Vector(0.,0.,1.));
    currentinfo->node.loc.att.icrf.utc = currentutc;
    currentinfo->node.loc.att.icrf.s = drotate_between(-Vector(currentinfo->node.loc.pos.extra.sun2earth.s), optimum).conjugate().to_q();
    currentinfo->node.loc.att.icrf.v = rv_zero();
    currentinfo->node.loc.att.icrf.a = rv_zero();
    ++currentinfo->node.loc.att.icrf.pass;
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);
    att_icrf(currentinfo->node.loc);
    initialloc.att = currentinfo->node.loc.att;

    return  0;
}

int32_t SolarAttitudePropagator::Reset(double nextutc)
{
    currentinfo->node.loc.att = initialloc.att;
    currentutc = currentinfo->node.loc.att.utc;
    Propagate(nextutc);

    return  0;
}

int32_t SolarAttitudePropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }

    currentutc = nextutc;
    currentinfo->node.loc.att.icrf.utc = nextutc;
    currentinfo->node.loc.att.icrf.s = drotate_between(-Vector(currentinfo->node.loc.pos.extra.sun2earth.s), optimum).conjugate().to_q();
    currentinfo->node.loc.att.icrf.v = rv_zero();
    currentinfo->node.loc.att.icrf.a = rv_zero();
    ++currentinfo->node.loc.att.icrf.pass;
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);
    att_icrf(currentinfo->node.loc);

    return 0;
}

int32_t TargetAttitudePropagator::Init()
{
    return 0;
}

int32_t TargetAttitudePropagator::Propagate(double nextutc)
{
    const double range_limit = 3000000.;
    double range = range_limit;
    // Closest target
    targetstruc ctarget;
    for (targetstruc target : currentinfo->target)
    {
        if (target.elto > 0 && target.range < range)
        {
            range = target.range;
            ctarget = target;
        }
    }
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentutc = nextutc;
    if (range < range_limit)
    {
        rvector targ = ctarget.loc.pos.geoc.s - currentinfo->node.loc.pos.geoc.s;
        quaternion qt = q_drotate_between_rv(rv_unitz(1.), targ);
        currentinfo->node.loc.att.geoc.s = qt;
        currentinfo->node.loc.att.geoc.pass++;
        att_geoc(currentinfo->node.loc);
        //        cartpos cpointing;
        //        cpointing.s = drotate(currentinfo->node.loc.att.geoc.s, rv_unitz(-currentinfo->node.loc.pos.geod.s.h));
        //        double angle = sep_rv(cpointing.s, -currentinfo->node.loc.pos.geoc.s);
        //        cpointing.s = cpointing.s / cos(angle);
        //        cpointing.s += currentinfo->node.loc.pos.geoc.s;
        //        geoidpos gpointing;
        //        geoc2geod(cpointing, gpointing);
        //        double sep;
        //        geod2sep(gpointing.s, ctarget.loc.pos.geod.s, sep);
        //        double sep2 = sep * sep;

        //        // +Z vector in rotated frame
        //        Vector eci_z = Vector(rv_sub(ctarget.loc.pos.eci.s, currentinfo->node.loc.pos.eci.s));
        //        // Desired Y is cross product of Desired Z and velocity vector
        //        Vector eci_y = eci_z.cross(Vector(currentinfo->node.loc.pos.eci.v));

        //        currentinfo->node.loc.att.topo.s = q_fmult(q_change_around_x(ctarget.elto),q_change_around_z(ctarget.azto));
        //        currentinfo->node.loc.att.topo.v = rv_zero();
        //        currentinfo->node.loc.att.topo.a = rv_zero();
        //        currentinfo->node.loc.att.topo.utc = currentutc;
        //        currentinfo->node.loc.att.topo.pass++;
        //        att_topo(currentinfo->node.loc);
    }
    else
    {
        currentinfo->node.loc.att.lvlh.s = q_eye();
        currentinfo->node.loc.att.lvlh.v = rv_zero();
        currentinfo->node.loc.att.lvlh.a = rv_zero();
        currentinfo->node.loc.att.lvlh.utc = currentutc;
        currentinfo->node.loc.att.lvlh.pass++;
        att_lvlh(currentinfo->node.loc);
    }
    //    currentinfo->node.loc.att.icrf.pass++;
    //    att_icrf(currentinfo->node.loc);
    AttAccel(currentinfo->node.loc, currentinfo->node.phys);
    return 0;
}

int32_t TargetAttitudePropagator::Reset(double nextutc)
{
    return 0;
}

int32_t ThermalPropagator::Init(float temp)
{
    currentinfo->node.phys.temp = temp;
    currentinfo->node.phys.heat = currentinfo->node.phys.temp * currentinfo->node.phys.mass * currentinfo->node.phys.hcap;
    for (trianglestruc& triangle : currentinfo->node.phys.triangles)
    {
        triangle.temp = currentinfo->node.phys.temp;
        triangle.heat = triangle.temp * (triangle.mass * triangle.hcap);
    }
    return 0;
}

int32_t ThermalPropagator::Reset(float temp)
{
    currentutc = currentinfo->node.loc.utc;
    if (temp == 0.f)
    {
        currentinfo->node.phys.temp = initialphys.temp;
    }
    else
    {
        currentinfo->node.phys.temp = temp;
    }
    currentinfo->node.phys.heat = currentinfo->node.phys.temp * currentinfo->node.phys.mass * currentinfo->node.phys.hcap;
    for (trianglestruc& triangle : currentinfo->node.phys.triangles)
    {
        triangle.temp = currentinfo->node.phys.temp;
        triangle.heat = triangle.temp * (triangle.mass * triangle.hcap);
    }
    return 0;
}

int32_t ThermalPropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        double energyd = 0.;
        double heatratio = currentinfo->node.phys.heat / currentinfo->node.phys.mass;
        currentinfo->node.phys.heat = 0.;
        //                double ienergyd = currentinfo->node.phys.radiation / currentinfo->node.phys.mass;
        currentinfo->node.phys.radiation = 0.;
        double atotal = 0.;
        double rtotal = 0.;
        for (trianglestruc& triangle : currentinfo->node.phys.triangles)
        {
            triangle.heat = heatratio * triangle.mass;
            triangle.temp = triangle.heat / (triangle.mass * triangle.hcap);

            // First do all inputs

            if (triangle.external)
            {
                energyd = (triangle.sirradiation + triangle.eirradiation) * dt;
                triangle.heat += ((1. - triangle.pcell) * triangle.abs + triangle.pcell * .7) * energyd;
                atotal += ((1. - triangle.pcell) * triangle.abs + triangle.pcell * .7) * energyd;
                // Area not covered with cells
                //                        if (triangle.pcell > 0.)
                //                        {
                //                            triangle.heat += (1. - triangle.pcell) * triangle.abs * energyd;
                //                            // Area covered with cells
                //                            if (triangle.ecellbase > 0.)
                //                            {
                //                                double efficiency = triangle.ecellbase + triangle.ecellslope * triangle.temp;
                //                                triangle.heat += (triangle.pcell) * triangle.abs * (1. - efficiency) * energyd;
                //                            }
                //                            else
                //                            {
                //                                triangle.heat += triangle.pcell * triangle.abs * energyd;
                //                            }
                //                        }
                //                        else
                //                        {
                //                            triangle.heat += triangle.abs * energyd;
                //                        }

                // Add back in previous radiation
                //                        triangle.heat += triangle.mass * ienergyd / 2.;
            }
            //                    else
            //                    {
            //                        // Internal Normal faces
            //                        triangle.heat += triangle.mass * ienergyd;
            //                    }

            // Then do outputs
            energyd = triangle.area * dt * SIGMA * pow(triangle.temp ,4);
            if (triangle.external)
            {
                // External
                triangle.heat -= 1.05 * triangle.emi * energyd;
                rtotal += 1.05 * triangle.emi * energyd;
                currentinfo->node.phys.radiation += 1.95 * triangle.iemi * energyd;
            }
            //                    else
            //                    {
            //                        // Purely internal
            //                        triangle.heat -= triangle.iemi * energyd;
            //                        currentinfo->node.phys.radiation += triangle.iemi * energyd;
            //                    }

            triangle.temp = triangle.heat / (triangle.mass * triangle.hcap);
            currentinfo->node.phys.heat += triangle.heat;
        }

        currentinfo->node.phys.temp = heatratio / currentinfo->node.phys.hcap;
        temperature = currentinfo->node.phys.temp;
    }

    return 0;
}

int32_t ElectricalPropagator::Init(float bp)
{
    battery_charge = bp;
    return 0;
}

int32_t ElectricalPropagator::Reset(float bp)
{
    currentutc = currentinfo->node.loc.utc;
    if (bp == 0.f)
    {
        currentinfo->node.phys.battlev = initialphys.battlev;
    }
    else
    {
        currentinfo->node.phys.battlev = bp;
    }
    return  0;
}

int32_t ElectricalPropagator::Propagate(double nextutc)
{
    double dcharge;
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        currentinfo->node.phys.powgen = 0.;
        for (trianglestruc& triangle : currentinfo->node.phys.triangles)
        {
            if (triangle.external)
            {
                if (triangle.pcell > 0.)
                {
                    if (triangle.ecellbase > 0.)
                    {
                        double efficiency = triangle.ecellbase + triangle.ecellslope * triangle.temp;
                        triangle.power = efficiency * triangle.sirradiation;
                        triangle.volt = triangle.vcell;
                        triangle.amp = -triangle.power / triangle.volt;
                        currentinfo->node.phys.powgen += triangle.power;
                    }
                }
            }
            else
            {
                triangle.power = 0.;
                triangle.volt = 0.;
                triangle.amp = 0.;
            }
        }
        currentinfo->node.phys.powuse = 0.;
        for (devicestruc* dev : currentinfo->device)
        {
            if (dev->state)
            {
                currentinfo->node.phys.powuse += dev->amp * dev->volt;
            }
        }
        double pdelta = currentinfo->node.phys.powuse-currentinfo->node.phys.powgen;
        if (currentinfo->devspec.batt_cnt)
        {
            double vsys = 0.;
            double vmax = currentinfo->devspec.batt_cnt * currentinfo->devspec.batt[0].maxvolt;
            double vmin = currentinfo->devspec.batt_cnt * currentinfo->devspec.batt[0].minvolt;
            double vbat = vmin + (vmax - vmin) * currentinfo->node.phys.battlev / currentinfo->node.phys.battcap;
            // Consuming
            if (pdelta > 0.)
            {
                vsys = vbat - sqrt(pdelta * currentinfo->devspec.batt[0].rout);
                if (currentinfo->devspec.bcreg_cnt)
                {
                    currentinfo->devspec.bcreg[0].mpptin_volt = 0.;
                    currentinfo->devspec.bcreg[0].mpptout_volt = vsys;
                    currentinfo->devspec.bcreg[0].volt = vsys;
                }
                if (vsys > vmax)
                {
                    vsys = vmax;
                }
                dcharge = -dt * (vsys - vbat) * (vsys - vbat) / currentinfo->devspec.batt[0].rout;
            }
            // Charging
            else
            {
                vsys = vbat + sqrt(-pdelta * currentinfo->devspec.batt[0].rin);
                if (currentinfo->devspec.bcreg_cnt)
                {
                    currentinfo->devspec.bcreg[0].mpptin_volt = vsys;
                    currentinfo->devspec.bcreg[0].mpptout_volt = 0.;
                    currentinfo->devspec.bcreg[0].volt = vsys;
                }
                if (vsys > vmax)
                {
                    vsys = vmax;
                }
                dcharge = dt * (vsys - vbat) * (vsys - vbat) / currentinfo->devspec.batt[0].rin;
            }
            for (uint16_t i=0; i<currentinfo->devspec.batt_cnt; i++)
            {
                currentinfo->devspec.batt[i].volt = vbat;
                currentinfo->devspec.batt[i].amp = -dcharge / dt;
                currentinfo->node.phys.battlev += dcharge;
                currentinfo->devspec.batt[i].charge += dcharge;
                if (currentinfo->devspec.batt[i].charge > currentinfo->devspec.batt[i].capacity)
                    currentinfo->devspec.batt[i].charge = currentinfo->devspec.batt[i].capacity;
                if (currentinfo->devspec.batt[i].charge < 0.)
                    currentinfo->devspec.batt[i].charge = 0.;
                currentinfo->devspec.batt[i].utc = currentinfo->node.loc.utc;
            }

            if (currentinfo->node.phys.powgen > currentinfo->node.phys.powuse)
                currentinfo->node.flags |= NODE_FLAG_CHARGING;
            else
                currentinfo->node.flags &= ~NODE_FLAG_CHARGING;

            if (currentinfo->node.phys.battlev < 0.)
                currentinfo->node.phys.battlev = 0.;

            if (currentinfo->node.phys.battlev >= currentinfo->node.phys.battcap)
            {
                currentinfo->node.flags &= ~NODE_FLAG_CHARGING;
                currentinfo->node.phys.battlev = currentinfo->node.phys.battcap;
            }
        }

    }
    return 0;
}

int32_t OrbitalEventGenerator::Init()
{
    umbra_start = 0.;
    gs_AoS.clear();
    return 0;
}

int32_t OrbitalEventGenerator::Reset()
{
    currentutc = currentinfo->node.loc.utc;
    umbra_start = 0.;
    gs_AoS.clear();
    return 0;
}

int32_t OrbitalEventGenerator::Propagate(double nextutc)
{
    currentinfo->event.clear();
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;

        check_umbra_event(false);
        check_target_events(false);
    }

    return 0;
}

int32_t OrbitalEventGenerator::End()
{
    // Force event end if active
    check_umbra_event(true);
    check_target_events(true);
    return 0;
}

int32_t MetricGenerator::Init()
{
    return 0;
}

//int32_t MetricGenerator::AddDetector(float fov, float ifov, float specmin, float specmax)
//{
//    camstruc det;
//    det.fov = fov;
//    det.ifov = ifov;
//    det.specmin = specmin;
//    det.specmax = specmax;
//    currentinfo->devspec.cam.push_back(det);
//    return currentinfo->devspec.cam.size();
//}

int32_t MetricGenerator::Propagate(double nextutc)
{
    if (coverage.size() != currentinfo->target.size())
    {
        coverage.resize(currentinfo->target.size());
        for (uint16_t i=0; i<currentinfo->target.size(); ++i)
        {
            coverage[i].resize(currentinfo->devspec.cam.size());
        }
    }

    //    bool printed = false;
    for (uint16_t it=0; it<currentinfo->target.size(); ++it)
    {
        if (currentinfo->target[it].elto > 0.)
        {
            for (uint16_t id=0; id<currentinfo->devspec.cam.size(); ++id)
            {
                double h = currentinfo->node.loc.pos.geod.s.h;
                double h2 = h * h;
                double nadirradius = h * currentinfo->devspec.cam[id].fov / 2.;
                cartpos cpointing;
                cpointing.s = drotate(currentinfo->node.loc.att.geoc.s, currentinfo->devspec.cam[id].los);
                double angle = sep_rv(cpointing.s, -currentinfo->node.loc.pos.geoc.s);
                double t1a = tan(angle);
                double t2a = t1a * t1a;
                double t4a = t2a * t2a;
                double r = Rearth(currentinfo->node.loc.pos.geod.s.lat);
                double lsep = (sqrt(t4a * (-4. * h2 - 8. * r * h) + t2a * 4. * r * r) + 2. * t2a * (h + r)) / (2. * (t2a + 1.));
                //                lsep = (sqrt(t4a * (-4. * h2 - 8. * r * h) + t2a * 4. * r * r) + 2. * t2a * (h + r)) / (2. * (t2a + 1.));
                cpointing.s = cpointing.s * lsep / cos(angle);
                cpointing.s += currentinfo->node.loc.pos.geoc.s;
                geoidpos gpointing;
                geoc2geod(cpointing, gpointing);
                coverage[it][id].area = 0.;
                double dr = nadirradius / sin(currentinfo->target[it].elto);
                double dr2 = dr * dr;
                double sep;
                geod2sep(gpointing.s, currentinfo->target[it].loc.pos.geod.s, sep);
                double sep2 = sep * sep;
                double tr = sqrt(currentinfo->target[it].area / DPI);
                double tr2 = tr * tr;
                if (sep < dr + tr)
                {
                    coverage[it][id].elevation = currentinfo->target[it].elto;
                    coverage[it][id].azimuth = currentinfo->target[it].azto;
                    coverage[it][id].resolution = currentinfo->target[it].range * currentinfo->devspec.cam[id].ifov;
                    coverage[it][id].specmin = currentinfo->devspec.cam[id].specmin;
                    coverage[it][id].specmax = currentinfo->devspec.cam[id].specmax;
                    if (sep > fabs(tr - dr))
                    {
                        coverage[it][id].area = dr2 * acos((sep2 + dr2 - tr2) / (2 * sep * dr)) + tr2 * acos((sep2 + tr2 - dr2) / (2 * sep * tr));
                        coverage[it][id].area -= sqrt((-sep + tr + dr) * (sep + tr - dr) * (sep - tr + dr) * (sep + tr + dr)) / 2.;
                        coverage[it][id].area *= sin(currentinfo->target[it].elto);
                    }
                    else
                    {
                        coverage[it][id].area = dr < tr ? DPI * dr2 * sin(currentinfo->target[it].elto) : currentinfo->target[it].area;
                    }
                }
                if (coverage[it][id].area < currentinfo->target[it].area)
                {
                    coverage[it][id].percent = coverage[it][id].area / currentinfo->target[it].area;
                }
                else
                {
                    coverage[it][id].percent = 1.;
                }
            }
        }
    }
    //    if (printed)
    //    {
    //        printf("\n");
    //    }
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentutc = nextutc;
    return 0;
}

int32_t MetricGenerator::Reset(double nextutc)
{
    return 0;
}

void OrbitalEventGenerator::check_umbra_event(bool force_end)
{
    // Umbra start
    if (umbra_start == 0. && !currentinfo->node.loc.pos.sunradiance)
    {
        // Add umbra to event list
        eventstruc umbra_event;
        umbra_event.name = "UMBRAIN";
        umbra_event.type = EVENT_TYPE_UMBRA;
        umbra_event.utc = umbra_start;
        umbra_event.dtime = currentutc - umbra_start;
        currentinfo->event.push_back(umbra_event);
        umbra_start = currentutc;
    }
    // Umbra end
    else if (umbra_start != 0. && (currentinfo->node.loc.pos.sunradiance || force_end))
    {
        // Add umbra to event list
        eventstruc umbra_event;
        umbra_event.name = "UMBRAOUT";
        umbra_event.type = EVENT_TYPE_UMBRA;
        umbra_event.utc = umbra_start;
        umbra_event.dtime = currentutc - umbra_start;
        currentinfo->event.push_back(umbra_event);
        umbra_start = 0.;
    }
}

void OrbitalEventGenerator::check_target_events(bool force_end)
{
    for (auto target = currentinfo->target.begin(); target != currentinfo->target.end(); target++)
    {
        if (target->type == NODE_TYPE_GROUNDSTATION)
        {
            check_gs_aos_event(*target, force_end);
        }
        else if (target->type == NODE_TYPE_TARGET)
        {
            check_target_aos_event(*target, force_end);
        }
    }
}

void OrbitalEventGenerator::check_gs_aos_event(const targetstruc& gs, bool force_end)
{
    // Find target sight acquisition/loss
    // Groundstation is in line-of-sight if elto (elevation from target to sat) is positive
    double elto_deg = DEGOF(gs.elto);
    if (gs_AoS.find(gs.name) == gs_AoS.end())
    {
        gs_AoS[gs.name] = {std::make_pair(0.,0.f),std::make_pair(0.,0.f),std::make_pair(0.,0.f),std::make_pair(0.,0.f)};
    }
    target_aos_set& gsAOS = gs_AoS[gs.name];
    // Update highest elevation values for 0/5/10 degree events
    for (size_t i=0; i<gsAOS.size()-1; ++i)
    {
        // gsAOS[i].first is the mjd timestamp of when the event began
        // gsAOS[i].second is the max elevation for this AoS event

        gsAOS[i].second = std::max(gsAOS[i].second, gs.elto);
        // AoS
        if (elto_deg > i*5. && gsAOS[i].first == 0.)
        {
            eventstruc gs_aos_event;
            gs_aos_event.name = GS_EVENT_STRING[i] + "AOS_" + gs.name;
            gs_aos_event.type = GS_EVENT_CODE[i];
            gs_aos_event.utc = gsAOS[i].first;
            gs_aos_event.dtime = currentutc - gsAOS[i].first;
            gs_aos_event.value = gsAOS[i].second;
            currentinfo->event.push_back(gs_aos_event);
            gsAOS[i].first = currentutc;
        }
        // LoS
        else if ((force_end || elto_deg <= i*5.) && gsAOS[i].first != 0.)
        {
            // Add event to event list
            eventstruc gs_aos_event;
            gs_aos_event.name = GS_EVENT_STRING[i] + "LOS_" + gs.name;
            gs_aos_event.type = GS_EVENT_CODE[i];
            gs_aos_event.utc = gsAOS[i].first;
            gs_aos_event.dtime = currentutc - gsAOS[i].first;
            gs_aos_event.value = gsAOS[i].second;
            currentinfo->event.push_back(gs_aos_event);
            // Reset this AoS event
            gsAOS[i].first = 0.;
            gsAOS[i].second = 0.f;
        }
    }
    // MAXDEG event
    // AoS
    if (gs.elto > 0. && gs.elto > gsAOS[DEGMAX].second)
    {
        // Keep track of when the max elevation was achieved
        gsAOS[DEGMAX].first = currentutc;
        gsAOS[DEGMAX].second = gs.elto;
    }
    // LoS
    else if ((force_end || gs.elto <= 0.) && gsAOS[DEGMAX].first != 0.)
    {
        // Add event to event list
        eventstruc gs_aos_event;
        gs_aos_event.name = GS_EVENT_STRING[DEGMAX] + "_" + gs.name;
        gs_aos_event.type = GS_EVENT_CODE[DEGMAX];
        gs_aos_event.utc = gsAOS[DEGMAX].first;
        gs_aos_event.dtime = 0;
        gs_aos_event.value = gsAOS[DEGMAX].second;
        currentinfo->event.push_back(gs_aos_event);
        // Reset this AoS event
        gsAOS[DEGMAX].first = 0.;
        gsAOS[DEGMAX].second = 0.f;
    }
}

void OrbitalEventGenerator::check_target_aos_event(const targetstruc& target, bool force_end)
{
    // Find target sight acquisition/loss
    // Target is in line-of-sight if elto (elevation from target to sat) is positive
    double elto_deg = DEGOF(target.elto);
    if (target_AoS.find(target.name) == target_AoS.end())
    {
        target_AoS[target.name] = {std::make_pair(0.,0.f)};
    }
    aos_pair& tAOS = target_AoS[target.name];
    aos_pair& tMAX = target_AoS[target.name];
    // Update highest elevation values for 0/5/10 degree events
    // tAOS[i].first is the mjd timestamp of when the event began
    // tAOS[i].second is the max elevation for this AoS event

    tAOS.second = std::max(tAOS.second, target.elto);
    // AoS
    if (elto_deg > 0. && tAOS.first == 0.)
    {
        eventstruc target_aos_event;
        target_aos_event.name = "TAOS_" + target.name;
        target_aos_event.type = EVENT_TYPE_TARG;
        target_aos_event.utc = tAOS.first;
        target_aos_event.dtime = currentutc - tAOS.first;
        target_aos_event.value = tAOS.second;
        target_aos_event.az = target.azto;
        currentinfo->event.push_back(target_aos_event);
        tAOS.first = currentutc;
    }
    // LoS
    else if ((force_end || elto_deg <= 0.) && tAOS.first != 0.)
    {
        // Add event to event list
        eventstruc target_aos_event;
        target_aos_event.name = "TLOS_" + target.name;
        target_aos_event.type = EVENT_TYPE_TARG;
        target_aos_event.utc = tAOS.first;
        target_aos_event.dtime = currentutc - tAOS.first;
        target_aos_event.value = tAOS.second;
        target_aos_event.az = target.azto;
        currentinfo->event.push_back(target_aos_event);
        // Reset this AoS event
        tAOS.first = 0.;
        tAOS.second = 0.f;
    }
    // MAXDEG event
    // AoS
    if (elto_deg > 0. && elto_deg > tMAX.second)
    {
        // Keep track of when the max elevation was achieved
        tMAX.first = currentutc;
        tMAX.second = elto_deg;
    }
    // MAX
    else if ((force_end || elto_deg <= 0.) && tMAX.first != 0.)
    {
        // Add event to event list
        eventstruc target_max_event;
        target_max_event.name = "TMAX_" + target.name;
        target_max_event.type = EVENT_TYPE_TARG;
        target_max_event.utc = tMAX.first;
        target_max_event.dtime = 0;
        target_max_event.value = tMAX.second;
        target_max_event.az = target.azto;
        currentinfo->event.push_back(target_max_event);
        // Reset this event
        tMAX.first = 0.;
        tMAX.second = 0.f;
    }
}

int32_t InertialPositionPropagator::Init()
{
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t InertialPositionPropagator::Reset(double nextutc)
{
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    Propagate(nextutc);

    return 0;
}

int32_t InertialPositionPropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentinfo->node.loc.pos.icrf = initialloc.pos.icrf;
    currentutc = nextutc;
    currentinfo->node.loc.pos.icrf.utc = nextutc;
    currentinfo->node.loc.pos.icrf.pass++;
    pos_icrf(currentinfo->node.loc);
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t LvlhPositionPropagator::Init()
{
    pos_origin2lvlh(currentinfo->node.loc);
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t LvlhPositionPropagator::Init(cartpos lvlh)
{
    currentinfo->node.loc.pos.lvlh = lvlh;
    pos_origin2lvlh(currentinfo->node.loc);
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t LvlhPositionPropagator::Reset(locstruc &loc)
{
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    Propagate(loc);

    return 0;
}

int32_t LvlhPositionPropagator::Propagate(locstruc &loc)
{
    double nextutc = loc.pos.geod.utc;
    pos_lvlh2origin(currentinfo->node.loc);
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        currentinfo->node.loc.pos.lvlh.utc = currentutc;
        currentinfo->node.loc.pos.lvlh.a += dt * currentinfo->node.loc.pos.lvlh.j;
        currentinfo->node.loc.pos.lvlh.v += dt * (currentinfo->node.loc.pos.lvlh.a + (dt / 2.) * currentinfo->node.loc.pos.lvlh.j);
        currentinfo->node.loc.pos.lvlh.s += dt * (currentinfo->node.loc.pos.lvlh.v + dt * ((1/2.) * currentinfo->node.loc.pos.lvlh.a + dt * (1.6) * currentinfo->node.loc.pos.lvlh.j));
    }
    currentinfo->node.loc.tle.name = "";
    currentinfo->node.loc = loc;
    pos_origin2lvlh(currentinfo->node.loc);

    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t GeoPositionPropagator::Init()
{
    return 0;
}

int32_t GeoPositionPropagator::Reset(double nextutc)
{
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    Propagate(nextutc);

    return 0;
}

int32_t GeoPositionPropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    currentinfo->node.loc.pos.geod = initialloc.pos.geod;
    currentutc = nextutc;
    currentinfo->node.loc.pos.geod.utc = nextutc;
    currentinfo->node.loc.pos.geod.pass = currentinfo->node.loc.pos.eci.pass + 1;
    pos_geod(currentinfo->node.loc);
    //            PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t IterativePositionPropagator::Init()
{
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t IterativePositionPropagator::Reset(double nextutc)
{
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    Propagate(nextutc);

    return 0;
}

int32_t IterativePositionPropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        currentinfo->node.loc.pos.eci.utc = currentutc;
        currentinfo->node.loc.pos.eci.s += dt * (currentinfo->node.loc.pos.eci.v + dt * ((1/2.) * currentinfo->node.loc.pos.eci.a + dt * (1.6) * currentinfo->node.loc.pos.eci.j));
        currentinfo->node.loc.pos.eci.v += dt * (currentinfo->node.loc.pos.eci.a + (dt / 2.) * currentinfo->node.loc.pos.eci.j);
        currentinfo->node.loc.pos.eci.a += dt * currentinfo->node.loc.pos.eci.j;

        //        rvector ds = rv_smult(.5 * dt * dt, currentinfo->node.loc.pos.eci.a);
        //        ds = rv_add(ds, rv_smult(dt, currentinfo->node.loc.pos.eci.v));
        //        currentinfo->node.loc.pos.eci.s = rv_add(currentinfo->node.loc.pos.eci.s, ds);
        //        currentinfo->node.loc.pos.eci.v = rv_add(currentinfo->node.loc.pos.eci.v, rv_smult(dt, currentinfo->node.loc.pos.eci.a));
        //        currentinfo->node.loc.pos.eci.utc = currentutc;

        // Update acceleration for the new position
        PosAccel(currentinfo->node.loc, currentinfo->node.phys);
        // Apply external accelerations
        // currentinfo->node.loc.pos.eci.a = rv_add(currentinfo->node.loc.pos.eci.a, rv_smult(1./currentinfo->node.phys.mass, currentinfo->node.phys.fpush.to_rv()));
        // Clearing external accelerations TODO: consider if this is desireable
        // currentinfo->node.phys.fpush.clear();
        // currentinfo->node.loc.pos.eci.pass++;
        // pos_eci(currentinfo->node.loc);
    }

    return 0;
}

//int32_t TlePositionPropagator::Init(tlestruc tle)
//{
//    tle2eci(currentutc, currentinfo->node.loc.tle, currentinfo->node.loc.pos.eci);
//    currentinfo->node.loc.pos.eci.pass++;
//    PosAccel(currentinfo->node.loc, currentinfo->node.phys);
//    pos_eci(currentinfo->node.loc);

//    return 0;
//}

int32_t TlePositionPropagator::Init()
{
    //    eci2tle(currentutc, currentinfo->node.loc.pos.eci, tle);
    //    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    return 0;
}

int32_t TlePositionPropagator::Reset(double nextutc)
{
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    Propagate(nextutc);

    return 0;
}

int32_t TlePositionPropagator::Propagate(double nextutc)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }
    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        tle2eci(currentutc, currentinfo->node.loc.tle, currentinfo->node.loc.pos.eci);
        currentinfo->node.loc.pos.eci.pass++;
        pos_eci(currentinfo->node.loc);
        PosAccel(currentinfo->node.loc, currentinfo->node.phys);
    }

    return 0;
}

int32_t GaussJacksonPositionPropagator::Setup()
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
    // order = 0;

    dtsq = dt * dt;

    order2 = order/2;
    //            order = order2 * 2;

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

int32_t GaussJacksonPositionPropagator::Init(tlestruc tle)
{
    int32_t iretn = 0;

    loc_clear(step[order+1].loc);
    tle2eci(currentinfo->node.loc.utc, tle, currentinfo->node.loc.pos.eci);
    ++currentinfo->node.loc.pos.eci.pass;
    pos_eci(currentinfo->node.loc);
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);
    //            AttAccel(currentinfo->node.loc, currentinfo->node.phys);
    step[order2].loc = currentinfo->node.loc;

    // Position at t0-dt
    for (uint32_t i=order2-1; i<order2; --i)
    {
        step[i].loc = step[i+1].loc;
        step[i].loc.utc -= dtj;
        tle2eci(step[i].loc.utc, tle, step[i].loc.pos.eci);
        step[i].loc.pos.eci.pass++;
        pos_eci(step[i].loc);

        step[i].loc.att.lvlh = step[i+1].loc.att.lvlh;
        att_lvlh2icrf(step[i].loc);

        PosAccel(step[i].loc, currentinfo->node.phys);
        AttAccel(step[i].loc, currentinfo->node.phys);
    }

    for (uint32_t i=order2+1; i<=order; i++)
    {
        step[i].loc = step[i-1].loc;

        step[i].loc.utc += dtj;
        tle2eci(step[i].loc.utc, tle, step[i].loc.pos.eci);
        step[i].loc.pos.eci.pass++;
        pos_eci(step[i].loc);

        step[i].loc.att.lvlh = step[i-1].loc.att.lvlh;
        att_lvlh2icrf(step[i].loc);

        PosAccel(step[i].loc, currentinfo->node.phys);
        AttAccel(step[i].loc, currentinfo->node.phys);
    }

    iretn = Converge();
    currentinfo->node.phys.utc = currentinfo->node.loc.utc;
    currentutc = currentinfo->node.loc.utc;

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
    ++currentinfo->node.loc.pos.eci.pass;
    pos_eci(currentinfo->node.loc);
    // Update accelerations
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

    initialloc = currentinfo->node.loc;
    initialphys = currentinfo->node.phys;
    currentinfo->node.phys.utc = currentinfo->node.loc.utc;

    // Zero out original N+1 bin
    loc_clear(step[order+1].loc);


    // Set central bin to initial state vector
    step[order2].loc = currentinfo->node.loc;

    // Position at t0-dt
    eci2kep(currentinfo->node.loc.pos.eci, kep);

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

        PosAccel(step[i].loc, currentinfo->node.phys);
    }

    eci2kep(currentinfo->node.loc.pos.eci, kep);

    // Initialize future bins
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

        PosAccel(step[i].loc, currentinfo->node.phys);
    }
    currentutc = step[order2].loc.utc;

    // Converge on rational set of values
    iretn = Converge();

    return iretn;
}

int32_t GaussJacksonPositionPropagator::Init(vector<locstruc> locs)
{
    int32_t iretn = 0;

    // Make sure ::locstruc is internally self consistent
    ++currentinfo->node.loc.pos.eci.pass;
    pos_eci(currentinfo->node.loc);

    // Zero out original N+1 bin
    loc_clear(step[order+1].loc);

    // Calculate initial accelerations
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);
    //            AttAccel(currentinfo->node.loc, currentinfo->node.phys);

    // Set central bin to initial state vector
    step[order2].loc = currentinfo->node.loc;

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

        PosAccel(step[i].loc, currentinfo->node.phys);
        //                AttAccel(&step[i].loc, currentinfo->node.phys);
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

        PosAccel(step[i].loc, currentinfo->node.phys);
        //                AttAccel(&step[i].loc, currentinfo->node.phys);
    }
    iretn = Converge();

    currentinfo->node.phys.utc = currentinfo->node.loc.utc;
    currentutc = step[order2].loc.utc;
    return iretn;
}

int32_t GaussJacksonPositionPropagator::Reset(double nextutc)
{
    int32_t iretn = 0;
    currentinfo->node.loc.pos = initialloc.pos;
    currentutc = currentinfo->node.loc.pos.utc;
    iretn = Init();
    Propagate(nextutc);
    return iretn;
}

int32_t GaussJacksonPositionPropagator::Propagate(double nextutc, quaternion icrf)
{
    if (nextutc == 0.)
    {
        nextutc = currentutc + dtj;
    }

    while ((nextutc - currentutc) > dtj / 2.)
    {
        currentutc += dtj;
        //                Vector normal, unitv, unitx, unitp, unitp1, unitp2;
        //                Vector lunitp1(.1,.1,0.);
        //                Vector tvector;

        // Don't bother if too low
        if (Vector(currentinfo->node.loc.pos.eci.s).norm() < REARTHM)
        {
            return GENERAL_ERROR_TOO_LOW;
        }

        step[order+1].loc.utc = step[order+1].loc.pos.utc = step[order+1].loc.pos.eci.utc = step[order].loc.pos.eci.utc + dtj;
        step[order+1].loc.att.icrf.s = icrf;

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

        // Update inherent accelerations for this location
        PosAccel(step[order+1].loc, currentinfo->node.phys);

        // Calculate s(order/2+1)
        step[order+1].s.col[0] = step[order].s.col[0] + (step[order].loc.pos.eci.a.col[0]+step[order+1].loc.pos.eci.a.col[0])/2.;
        step[order+1].s.col[1] = step[order].s.col[1] + (step[order].loc.pos.eci.a.col[1]+step[order+1].loc.pos.eci.a.col[1])/2.;
        step[order+1].s.col[2] = step[order].s.col[2] + (step[order].loc.pos.eci.a.col[2]+step[order+1].loc.pos.eci.a.col[2])/2.;

        // Shift everything over 1
        for (uint16_t j=0; j<=order; j++)
        {
            step[j] = step[j+1];
        }

        // Adjust for any thrust
        if (currentinfo->node.phys.fpush.norm() && currentinfo->node.phys.mass)
        {
            Vector dacc = (1./currentinfo->node.phys.mass) * currentinfo->node.phys.fpush;
            step[order2].loc.pos.eci.s = rv_add(step[order2].loc.pos.eci.s, 0.5 * dt2 * dacc.to_rv());
            step[order2].loc.pos.eci.v = rv_add(step[order2].loc.pos.eci.v, dt * dacc.to_rv());
            Update();
        }

    }

    currentinfo->node.loc.pos = step[order2].loc.pos;
    for (uint16_t i=order; i<=order; --i)
    {
        if (nextutc >= currentinfo->node.loc.pos.utc - dtj / 2.)
        {
            break;
        }
        currentinfo->node.loc.pos = step[i].loc.pos;
        currentinfo->node.loc.utc = currentinfo->node.loc.pos.utc;
    }

    return 0;
}

int32_t GaussJacksonPositionPropagator::Converge()
{
    uint32_t c_cnt, cflag=0, k;
    rvector oldsa;

    PosAccel(currentinfo->node.loc, currentinfo->node.phys);

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
                PosAccel(step[order2+i*n].loc, currentinfo->node.phys);

                // Compare acceleration at new position to previous iteration
                if (fabs(oldsa.col[0]-step[order2+i*n].loc.pos.eci.a.col[0])>1e-14 || fabs(oldsa.col[1]-step[order2+i*n].loc.pos.eci.a.col[1])>1e-14 || fabs(oldsa.col[2]-step[order2+i*n].loc.pos.eci.a.col[2])>1e-14)
                    cflag = 1;
            }
        }
        c_cnt++;
    } while (c_cnt<10 && cflag);

    currentinfo->node.loc = step[order2].loc;
    ++currentinfo->node.loc.pos.eci.pass;
    //    currentinfo->node.phys.fpush = rv_zero();
    PosAccel(currentinfo->node.loc, currentinfo->node.phys);
    pos_eci(currentinfo->node.loc);
    return 0;
}

int32_t GaussJacksonPositionPropagator::Update()
{
    int32_t iretn;
    kepstruc kep;
    double dea;
    quaternion q1;

    ++step[order2].loc.pos.eci.pass;
    pos_eci(step[order2].loc);
    PosAccel(step[order2].loc, currentinfo->node.phys);
    AttAccel(step[order2].loc, currentinfo->node.phys);

    // Set central bin to last bin
//    step[order2].loc = step[order].loc;
    eci2kep(step[order2].loc.pos.eci, kep);

    // Initialize past bins
    for (uint32_t i=order2-1; i<order2; --i)
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

        PosAccel(step[i].loc, currentinfo->node.phys);
    }

    eci2kep(step[order2].loc.pos.eci, kep);

    // Initialize future bins
    for (uint32_t i=order2+1; i<=order; i++)
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

        PosAccel(step[i].loc, currentinfo->node.phys);
    }

    Converge();

//    // Set central bin to first bin
//    step[order2].loc = step[0].loc;
//    eci2kep(step[order2].loc.pos.eci, kep);

//    // Initialize past bins
//    for (uint32_t i=order2-1; i<order2; --i)
//    {
//        step[i].loc = step[i+1].loc;
//        step[i].loc.utc -= dtj;
//        kep.utc = step[i].loc.utc;
//        kep.ma -= dt * kep.mm;

//        uint16_t count = 0;
//        do
//        {
//            dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
//            kep.ea -= dea;
//        } while (++count < 100 && fabs(dea) > .000001);
//        step[i].loc.pos.eci.utc = kep.utc;
//        kep2eci(kep, step[i].loc.pos.eci);
//        ++step[i].loc.pos.eci.pass;

//        q1 = q_axis2quaternion_rv(rv_smult(-dt,step[i].loc.att.icrf.v));
//        step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
//        normalize_q(&step[i].loc.att.icrf.s);
//        // Calculate new v from da
//        step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(-dt,step[i].loc.att.icrf.a));
//        step[i].loc.att.icrf.utc = kep.utc;
//        pos_eci(step[i].loc);

//        PosAccel(step[i].loc, currentinfo->node.phys);
//    }

//    eci2kep(step[order2].loc.pos.eci, kep);

//    // Initialize future bins
//    for (uint32_t i=order2+1; i<=order; i++)
//    {
//        step[i] = step[i-1];
//        step[i].loc.utc += dtj;
//        kep.utc = step[i].loc.utc;
//        kep.ma += dt * kep.mm;

//        uint16_t count = 0;
//        do
//        {
//            dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
//            kep.ea -= dea;
//        } while (++count < 100 && fabs(dea) > .000001);
//        step[i].loc.pos.eci.utc = kep.utc;
//        kep2eci(kep, step[i].loc.pos.eci);
//        ++step[i].loc.pos.eci.pass;

//        q1 = q_axis2quaternion_rv(rv_smult(dt,step[i].loc.att.icrf.v));
//        step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
//        normalize_q(&step[i].loc.att.icrf.s);
//        // Calculate new v from da
//        step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(dt,step[i].loc.att.icrf.a));
//        step[i].loc.att.icrf.utc = kep.utc;
//        pos_eci(step[i].loc);

//        PosAccel(step[i].loc, currentinfo->node.phys);
//    }

//    iretn = Converge();

    currentutc = step[order2].loc.utc;
    currentinfo->node.phys.utc = step[order2].loc.utc;
    return iretn;
}

}

}
