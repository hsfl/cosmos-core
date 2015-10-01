#include "physics/keplerianorbit.h"

Kepler::Kepler()
{
}


// algorithm 5, p77
double Kepler::nu2anomaly(double e, double nu)
{

    double Anomaly;
    double E, sinE, cosE; //  eccentric anomaly

    // for eliptical orbits, excluding circular ones
    if (e < 1.0){
        sinE = (sin(nu)*sqrt(1-e*e)) / (1+e*cos(nu));
        cosE = (e + cos(nu)) / (1+e*cos(nu));
        E = atan2(sinE,cosE);
        Anomaly = E;
    }

    // TOPO: add other cases for e=1.0 and e>1.0
    // ...
    return Anomaly;

}


// algorithm 6
double Kepler::anomaly2nu(double e, double E)
{

    double nu, sinNu, cosNu;  // true anomaly
    if (e < 1.0){
        // ?? the following two equations don't give exactly the same result
        sinNu = (sin(E)*sqrt(1-e*e)) / (1-e*cos(E)) ;
        cosNu = (cos(E) - e) / (1-e*cos(E)) ;
        nu = atan2(sinNu, cosNu);
        return nu;
    }
}


// Algorithm 9, Fundamentals of Astrodynamics and Applications, pg 114
// finding the orbital elements given position (r) and velocity vectors (v)
Keplerian Kepler::rv2coe(Vector3d v_r, Vector3d v_v)
{

    Constants c;
    //    Keplerian k;

    //    Vector3d v_r; // position vector
    //    Vector3d v_v; // velocity vector
    Vector3d v_h; // angular momentum vector
    Vector3d v_k; // unit vector (i,j,K)
    Vector3d v_n; // node vector
    Vector3d v_e; // eccentricity vector

    double r, v, h, n;
    double e; // eccentricity
    double xi; // specific mechanical energy
    double a; // semimajor axis
    double p; // specific angular momentum
    double i; // inclination
    double raan; // right ascension of the ascending node
    double omega; // argument of perigee
    double nu; // true anomaly

    //    cout << "Find classical orbital elements" << endl;
    r = v_r.norm();
    v = v_v.norm();


    // Find the specific angular momentum (h)
    v_h = v_r.cross(v_v);
    h = v_h.norm();// " km^2/s"

    // Find the node vector (not the mean motion) using a cross product;
    v_k << 0,0,1;
    v_n = v_k.cross(v_h);
    n = v_n.norm();
    v_e = (1/c.mu)*( (pow(v,2) - c.mu/r)*v_r - v_r.dot(v_v)*v_v);
    e = v_e.norm();
    xi = pow(v,2)/2-c.mu/r;

    if (e!=1.0){
        // for most orbits computing the semi major axis is just
        // a = - mu/(2*xi)
        // but for parabolic orbits a = infinite

        //    cout << "Because the orbit isn't parabolic (the eccentricity is not 1) find the semimajor axis" << endl;
        a = -c.mu / (2*xi); // km

        //    cout << "Then find the semiparameter suing the specific angular momentum" << endl;
        p = pow(h,2)/c.mu; //"km"
        //        p = a*(1-pow(e,2)); //"km"
    }

    //    cout << "The next task is to determine the angles. Find the inclination with a cosine expression. No quadrant check is necessary" << endl;
    i = acos(v_h(2)/h);
    //    cout << "i = " << i*180/M_PI << " deg" << endl << endl;

    //    cout << "Find the right ascencion of the acending node.
    // Notice the quadrant check here affects the final value" << endl;
    raan = acos(v_n(0)/n);
    if (v_n(1) < 0){
        raan = 2*M_PI - raan;
    }
    //    cout << "raan = " << raan*180/M_PI << " deg" << endl << endl;

    //    cout << "Find the argument of perigee similarly but without modification for quadrants" << endl;
    omega = acos(v_n.dot(v_e)/(n*e));
    if (v_e(2) < 0){
        omega = 2*M_PI - omega;
    }
    //    cout << "omega = " << omega*180/M_PI << " deg" << endl << endl;

    //    cout << "Finally find the true anomaly" << endl;
    nu = acos(v_e.dot(v_r)/(e*r));
    if (v_r.dot(v_v) < 0){
        nu = 2*M_PI - nu;
    }
    //    cout << "nu = " << nu*180/M_PI << " deg" << endl << endl;

    k.p = p;
    k.a = a;
    k.e = e;

    k.i = i;
    k.raan = raan;
    k.omega = omega;
    k.nu = nu;

    k.i_deg = i*180./M_PI;
    k.raan_deg = raan*180./M_PI;
    k.omega_deg = omega*180./M_PI;
    k.nu_deg = nu*180./M_PI;

    return k;
}


// Algorithm 10, Fundamentals of Astrodynamics and Applications, pg 118
// finding position (r) and velocity vectors (v) given classical orbital elements k
void Kepler::coe2rv(Keplerian k, CartesianVector &cv)
{

    Constants c;

    Vector3d v_r; // position vector
    Vector3d v_v; // velocity vector

    double p = k.p; // specific angular momentum
    double nu = k.nu; // true anomaly
    double e = k.e; // eccentricity
    double raan = k.raan; // right ascencion of the acending node
    double omega = k.omega; // eccentricity
    double i = k.i;


    // position vector
    double r_x = p*cos(nu)/(1 + e*cos(nu));
    double r_y = p*sin(nu)/(1 + e*cos(nu));
    double r_z = 0;

    v_r << r_x, r_y, r_z;

    //    cout << v_r << endl;

    // velocity vector
    double v_x = - sqrt(c.mu/p)*sin(nu);
    double v_y = sqrt(c.mu/p)*(e + cos(nu));
    double v_z = 0;

    v_v << v_x, v_y, v_z;

    //    cout << v_v << endl;

    Matrix3d m;
    // eigen apparently uses the rotation matrix transpose definition from Vallado
    // that's why we don't use the negative signs here
    m = AngleAxisd(raan, Vector3d::UnitZ()) *
            AngleAxisd(i, Vector3d::UnitX()) *
            AngleAxisd(omega, Vector3d::UnitZ());

    //    m = AngleAxisd(M_PI/4, Vector3d::UnitX()) ;
    //    cout << m << endl;
    //    cout << m << endl;

    // rotate position vector to the geocentric equatorial system (IJK)
    cv.r = m*v_r;

    // rotate velocity vector to the geocentric equatorial system (IJK)
    cv.v = m*v_v;

    //    cout << v_v << endl;

    //    cv.r = v_r;
    //    cv.v = v_v;
    //return k;
}



// compute eccentric anomaly form Mean Anomaly (M) and eccentricity (e)
double Kepler::KepEqtnE(double M, double e)
{

    double E, E_, error;
    if (-M_PI < M < 0 || M > M_PI){
        E = M - e;
    } else {
        E = M + e;
    }

    do {
        E_ = E + (M - E + e*sin(E))/(1.0-e*cos(E));
        error = fabs(E_ - E);
        E = E_;
    } while (error > 10e-8);

    return E_;
}

// overloaded function,
// this assumes you have set the cv_initial
void Kepler::KeplerCOE(double dt)
{
    KeplerCOE(cv_initial, dt, cv_final);

}


// algorithm 7, pg 81
// input: initial state vector (pos, vel) and dt
// output: final state vector (pos, vel)
void Kepler::KeplerCOE(CartesianVector &cv_initial, double dt, CartesianVector &cv_final)
{

    Constants c;
    double E_0; // intitial eccentric annomaly
    //    double dt;
    double n;
    double M_0;
    double M;
    double E;
    //    double nu;

    // get the keplerian elements from velocity and position vectors
    k = rv2coe(cv_initial.r, cv_initial.v);

    // mean motion (rad/sec)
    n = sqrt( c.mu /pow(k.a,3) );

    // orbital period
    //double T = 1.0/n;

    if (k.e != 0){
        E_0 = nu2anomaly(k.e, k.nu);
    }

    if (k.e < 1.0){
        M_0 = E_0 - k.e *sin(E_0);
        M = M_0 + n*dt;
    }

    // compute eccentric anomaly E
    E = KepEqtnE(M, k.e);

    if (k.e != 0){
        // update true anomaly
        k.nu = anomaly2nu(k.e, E);
        k.nu_deg = k.nu*180./M_PI;
    }

    //CartesianVector cv;
    coe2rv(k,cv_final);

}




// compute the satellite state vector in pseudo ECI
// TODO: convert code to use v_ecef in m not km
void Kepler::computePseudoEci(Vector3d r_ecef, Vector3d v_ecef)
{

    //    cout << "-------------------" << endl;
    //    cout << "compute pseudo ECI" << endl;
    //    cout.precision(15);



    //    // launch conditions for 1 May 2015 00:00:00.000 (from STK)
    //    // position and velocity vectors in ECEF -> pseudo intertial
    //    r_ecef << -6520.3856599118853000, -1875.5706668704402000, -563.4969816368675300;
    //    v_ecef << 0.2900946508525095, 1.2803388570418119, -7.6183105593596760;

    //    // launch conditions for 1 May 2015 09:24:49.000 (from STK)
    //    // at this particular time the ECEF == ECI
    //    // position and velocity vectors in ECEF -> pseudo intertial
    //    r_ecef << -6519.3363084791090000, -1881.8004898363615000, -554.8012246098181800;
    //    v_ecef << 0.4199558855770347, 0.7916859546158801, -7.6200781153309469;

    //    cout << "v ECI" << endl;
    //    cout << v_ecef.norm() << endl;

    // position given in ECEF
    // this is what is going to be given and won't change much
    // independent of time of launch
    //    r_ecef << -6518.5595286007429000, -1881.6252839422116000, -564.4389677880585600;
    //    v_ecef << 0.2941373237240016, 1.2666601089461893, -7.6194798496990126;

    // at this point the position in ECI and ECEF are the same
    r_peci = r_ecef;
    //    cout << "v ECEF" << endl;
    //    cout << v_ecef.norm() << endl;

    double earth_omega = 2.*M_PI/(24.*60.*60.);

    //  -------------------------
    // convert ECEF to pseudo-ECI
    Vector3d offsetVelocity;

    // compute the Earth velocity vector for this distance
    // as if the origin would be on the X axis (poiting towards Y)
    offsetVelocity << 0, earth_omega*r_ecef.norm(), 0;

    // now compute the velocity vector offset for the actual position
    // by rotating the vector by the actual angle of the satellite position
    // from the X vector
    double angle;
    float dot = 1.0*r_ecef[0] + 0.0*r_ecef[1];      // dot product
    float det = 1.0*r_ecef[1] - 0.0*r_ecef[0];      // determinant
    angle = atan2(det, dot);  // atan2(y, x) or atan2(sin, cos)

    //    cout << "angle: " << angle*180./M_PI << endl;

    // now add the rotation matrix corresponding to this angle
    Matrix3d m;
    m = AngleAxisd((float)angle, Vector3d::UnitZ());

    // compute the intial velocity in pseudo-ECI
    //    Vector3d v_peci;
    v_peci = m*offsetVelocity + v_ecef;

    //    cout << endl;
    //    cout << "Velocity pseudo-ECI:" << endl;
    //    cout << v_peci << endl;
    //    cout << "Velocity ECI mag:" ;
    //    cout << v_peci.norm() << endl;
    //    cout << v_ecef.norm() << endl;




    ////    cout << cv_final.r << endl << endl;
    ////    cout << cv_final.v << endl;

    //    Keplerian k = kepler.rv2coe(r_ecef, v_ecef);

    //    CartesianVector cv;
    //    kepler.coe2rv(k, cv);


    // setup the initial conditions for the KeplerCOE propagation
    cv_initial.r = r_peci;
    cv_initial.v = v_peci;
}


void Kepler::convertPseudoEciToGeodetic(double elapsedTime)
{
    // use the last computed value for the position vector
    // and use it as the pseudo ECI position
    r_peci = cv_final.r;
    convertPseudoEciToGeodetic(elapsedTime, r_peci);
}

// convert ECEF to Geodetic coordinates (LLA)
void Kepler::convertPseudoEciToGeodetic(double elapsedTime, Vector3d r_eci){
    // ----------------
    // convert vectors in pseudo ECI
    // to ECEF and then to to geocentric position (lat, lon, alt)
    //    Vector3d r; // position vector
    //    Vector3d v; // velocity vector

    double earth_omega = 2.*M_PI/(24.*60.*60.);

    // elapsed time since deployment
    //    double elapsedTime = 0; // sec

    double angleBetweenEciEcef;
    angleBetweenEciEcef = earth_omega*elapsedTime;

    Matrix3d m;
    m = AngleAxisd(-angleBetweenEciEcef, Vector3d::UnitZ());

    Vector3d r_ecef;
    r_ecef = m*r_eci;

    geocentric_alt = r_ecef.norm() - Constants::r_earth;
    geocentric_lat = -(acos(r_ecef[2]/(r_ecef.norm())) - M_PI/2.);
    geocentric_lon = atan2(r_ecef[1], r_ecef[0]);

//    cout << endl;
//    cout << "Geocentric position:" << endl ;

//    cout << "Alt   [km] : " << fixed << setprecision(3) << geocentric_r - Constants::r_earth<< endl;
//    cout << "Lat   [deg]: " << fixed << setprecision(2) << geocentric_lat*180/M_PI << endl;
//    cout << "Lon   [deg]: " << fixed << setprecision(2) << geocentric_lon*180/M_PI << endl;
//    cout << endl;
}



//// example from David Vallado Fundamentals of Astrodynamics and Applications, pg 114
//// finding the orbital elements given position (r) and velocity vectors (v)
//void Kepler::example_vallado_2_5(){

//    Constants c;

//    cout << "----------------------" << endl;
//    Vector3d v_r; // position vector
//    Vector3d v_v; // velocity vector
//    Vector3d v_h; // angular momentum vector
//    Vector3d v_k; // unit vector (i,j,K)
//    Vector3d v_n; // node vector
//    Vector3d v_e; // eccentricity vector

//    double r, v, h, n;
//    double e; // eccentricity
//    double xi; // specific mechanical energy
//    double a; // semimajor axis
//    double p; // specific angular momentum
//    double i; // inclination
//    double raan; // right ascension of the ascending node
//    double omega; // argument of perigee
//    double nu; // true anomaly

//    cout << "Given" << endl;
//    v_r << 6524.834, 6862.875, 6448.296;
//    v_v << 4.901327, 5.533756, -1.976341;
//    cout << "v_r = " << v_r.transpose() << endl;
//    cout << "v_v = " << v_v.transpose() << endl << endl;

//    cout << "Find classical orbital elements" << endl;
//    r = v_r.norm();
//    v = v_v.norm();
//    cout << "magnitutde r = " << r << endl;
//    cout << "magnitutde v = " << v << endl << endl;

//    cout << "Begin by finding the specific angular momentum" << endl;
//    v_h = v_r.cross(v_v);
//    h = v_h.norm();
//    cout << "v_h = " << v_h.transpose() << endl;
//    cout << "h = " << h << " km^2/s" << endl << endl;

//    cout << "Find the node vector (not the mean motion) using a cross product" << endl;
//    v_k << 0,0,1;
//    v_n = v_k.cross(v_h);
//    n = v_n.norm();
//    v_e = 1/c.mu*( (pow(v,2)-c.mu/r)*v_r - v_r.dot(v_v)*v_v);
//    e = v_e.norm();
//    xi = pow(v,2)/2-c.mu/r;

//    cout << "v_n = " << v_n.transpose() << endl;
//    cout << "n = " << n << " km^2/s" << endl;
//    cout << "v_e = " << v_e.transpose() << endl;
//    cout << "e = " << e << endl;
//    cout << "xi = " << xi << "km^2/s^2" << endl << endl;

//    cout << "Because the orbit isn't parabolic (the eccentricity is not 1) find the semimajor axis" << endl;
//    a = -c.mu / (2*xi);
//    cout << "a = " << a << "km" << endl << endl;

//    cout << "Then find the semiparameter suing the specific angular momentum" << endl;
//    p = pow(h,2)/c.mu;
//    cout << "p = " << p << "km" << endl << endl;

//    cout << "The next task is to determine the angles. Find the inclination with a cosine expression. No quadrant check is necessary" << endl;
//    i = acos(v_h(2)/h);
//    cout << "i = " << i*180/M_PI << " deg" << endl << endl;

//    cout << "Find the right ascencion of the acending node. Notice the quadrant check here affects the final value" << endl;
//    raan = acos(v_n(0)/n);
//    if (v_n(1) < 0){
//        raan = 2*M_PI - raan;
//    }
//    cout << "raan = " << raan*180/M_PI << " deg" << endl << endl;

//    cout << "Find the argument of perigee similarly but without modification for quadrants" << endl;
//    omega = acos(v_n.dot(v_e)/(n*e));
//    if (v_e(2) < 0){
//        omega = 2*M_PI - omega;
//    }
//    cout << "omega = " << omega*180/M_PI << " deg" << endl << endl;

//    cout << "Finally find the true anomaly" << endl;
//    nu = acos(v_e.dot(v_r)/(e*r));
//    if (v_r.dot(v_v) < 0){
//        nu = 2*M_PI - nu;
//    }
//    cout << "nu = " << nu*180/M_PI << " deg" << endl << endl;


//    // eccentric anomaly
//    double E = nu2anomaly(e,nu);
//    cout << "E = " << E << endl;

//    cout << "----------------------" << endl;
//}

//// example from David Vallado Fundamentals of Astrodynamics and Applications, pg 114
//// finding the orbital elements given position (r) and velocity vectors (v)
//void example_vallado_2_5_new(Vector3d v_r, Vector3d v_v, Keplerian k){

//    Constants c;

//    cout << "----------------------" << endl;
//    Vector3d v_h; // angular momentum vector
//    Vector3d v_k; // unit vector (i,j,K)
//    Vector3d v_n; // node vector
//    Vector3d v_e; // eccentricity vector

//    double r, v, h, n;

//    double xi; // specific mechanical energy

//    // Keplerian elements
//    double e = k.e; // eccentricity
//    double a = k.a; // semimajor axis
//    double p = k.p; // specific angular momentum
//    double i = k.i; // inclination
//    double raan = k.raan; // right ascension of the ascending node
//    double omega = k.omega; // argument of perigee
//    double nu = k.nu; // true anomaly

//    cout << "Given" << endl;
//    cout << "v_r = " << v_r.transpose() << endl;
//    cout << "v_v = " << v_v.transpose() << endl << endl;

//    cout << "Find classical orbital elements" << endl;
//    r = v_r.norm();
//    v = v_v.norm();
//    cout << "magnitutde r = " << r << endl;
//    cout << "magnitutde v = " << v << endl << endl;

//    cout << "Begin by finding the specific angular momentum" << endl;
//    v_h = v_r.cross(v_v);
//    h = v_h.norm();
//    cout << "v_h = " << v_h.transpose() << endl;
//    cout << "h = " << h << " km^2/s" << endl << endl;

//    cout << "Find the node vector (not the mean motion) using a cross product" << endl;
//    v_k << 0,0,1;
//    v_n = v_k.cross(v_h);
//    n = v_n.norm();
//    v_e = 1/c.mu*( (pow(v,2)-c.mu/r)*v_r - v_r.dot(v_v)*v_v);
//    e = v_e.norm();
//    xi = pow(v,2)/2-c.mu/r;

//    cout << "v_n = " << v_n.transpose() << endl;
//    cout << "n = " << n << " km^2/s" << endl;
//    cout << "v_e = " << v_e.transpose() << endl;
//    cout << "e = " << e << endl;
//    cout << "xi = " << xi << "km^2/s^2" << endl << endl;

//    cout << "Because the orbit isn't parabolic (the eccentricity is not 1) find the semimajor axis" << endl;
//    a = -c.mu / (2*xi);
//    cout << "a = " << a << "km" << endl << endl;

//    cout << "Then find the semiparameter suing the specific angular momentum" << endl;
//    p = pow(h,2)/c.mu;
//    cout << "p = " << p << "km" << endl << endl;

//    cout << "The next task is to determine the angles. Find the inclination with a cosine expression. No quadrant check is necessary" << endl;
//    i = acos(v_h(2)/h);
//    cout << "i = " << i*180/M_PI << " deg" << endl << endl;

//    cout << "Find the right ascencion of the acending node. Notice the quadrant check here affects the final value" << endl;
//    raan = acos(v_n(0)/n);
//    if (v_n(1) < 0){
//        raan = 2*M_PI - raan;
//    }
//    cout << "raan = " << raan*180/M_PI << " deg" << endl << endl;

//    cout << "Find the argument of perigee similarly but without modification for quadrants" << endl;
//    omega = acos(v_n.dot(v_e)/(n*e));
//    if (v_e(2) < 0){
//        omega = 2*M_PI - omega;
//    }
//    cout << "omega = " << omega*180/M_PI << " deg" << endl << endl;

//    cout << "Finally find the true anomaly" << endl;
//    nu = acos(v_e.dot(v_r)/(e*r));
//    if (v_r.dot(v_v) < 0){
//        nu = 2*M_PI - nu;
//    }
//    cout << "nu = " << nu*180/M_PI << " deg" << endl << endl;


//    cout << "----------------------" << endl;
//}



//void example_1(){
//    // source
//    // http://www.insight3d.com/resources/educational-alliance-program/astro-primer/primer75.htm

//    Vector3d v, v_initial, r_initial, r, g_vector;
//    v_initial << 10, 0, 0; // m/sec
//    double g = 9.81; // gravitational acceleration at the Earth's surface = 9.81 m/sec2
//    g_vector << 0, g, 0;

//    double t = 1;

//    // velocity
//    v = v_initial + g_vector*t;

//    // position
//    r = r_initial + v_initial*t + 0.5*g_vector*pow(t,2);

//    cout << "velocity" << endl << v << endl;
//    cout << "position" << endl << r << endl;

//}

