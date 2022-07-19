function [eo, xno, xmo, xincl, omegao, xnodeo] = rv2tle(reci, veci)
% convert osculating position and velocity vectors
% to components of two line element set (TLE)
% input
%  reci = eci position vector (kilometers)
%  veci = eci velocity vector (kiometers/second)
% output
%  eo     = orbital eccentricity (non-dimensional)
%  xno    = mean motion (orbits per day)
%  xmo    = mean anomaly (radians)
%  xincl  = orbital inclination (radians)
%  omegao = argument of perigee (radians)
%  xnodeo = right ascension of ascending node (radians)
% reference: Scott Campbell's Satellite Orbit Determination
%            web site www.satelliteorbitdetermination.com
% Orbital Mechanics with MATLAB
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
pi2 = 2.0 * pi;
xke = 0.0743669161331734132;
xj3 = -2.53881d-6;
req = 6378.135;
ck2 = 5.413079d-4;
a3ovk2 = -xj3 / ck2;
% convert position vector to earth radii
% and velocity vector to earth radii per minute
for i = 1:1:3
    
    rr2(i) = reci(i) / req;
    vv2(i) = 60.0 * veci(i) / req;
    vk(i) = vv2(i) / xke;
    
end
hv = cross(rr2, vk);
pl = dot(hv, hv);
vz(1) = 0.0;
vz(2) = 0.0;
vz(3) = 1.0;
vxh = cross(vz, hv);
if (vxh(1) == 0.0 && vxh(2) == 0.0)
    
    vxh(1) = 1.0;
    
end
vxhhat = vxh / norm(vxh);
rk = norm(rr2);
rdotv = dot(rr2, vv2);
rdotk = rdotv / rk;
hmag = norm(hv);
rfdotk = hmag * xke / rk;
vwrk = dot(rr2, vxhhat);
temp = vwrk / rk;
uk = acos(temp);
if (rr2(2) < 0.0)
    uk = pi2 - uk;
end
vz = cross(vk, hv);
for i = 1:1:3
    
    vy(i) = -1.0 * rr2(i) / rk;
    
end
for i = 1:1:3
    
    vec(i) = vz(i) + vy(i);
    
end
ek = norm(vec);
if (ek >= 1.0)
    
    return
    
end
xnodek = atan3(vxhhat(2), vxhhat(1));
temp = sqrt(hv(1) * hv(1) + hv(2) * hv(2));
xinck = atan2(temp, hv(3));
vwrk = dot(vec, vxhhat);
temp = vwrk / ek;
wk = acos(temp);
if (vec(3) < 0.0)
    
    wk = mod(pi2 - wk, pi2);
    
end
aodp = pl / (1.0 - ek * ek);
xn = xke * aodp^(-1.5);
% in the first loop the osculating elements rk, uk, xnodek, xinck, rdotk,
% and rfdotk are used as anchors to find the corresponding final sgp4
% mean elements r, u, xnodeo, xincl, rdot, and rfdot. several other final
% mean values based on these are also found: betal, cosio, sinio, theta2,
% cos2u, sin2u, x3thm1, x7thm1, x1mth2. in addition, the osculating values
% initially held by aodp, pl, and xn are replaced by intermediate
% (not osculating and not mean) values used by sgp4. the loop converges
% on the value of pl in about four iterations.
% seed value for first loop
xincl = xinck;
u = uk;
for i = 1:1:20
    a2 = pl;
    betal = sqrt(pl / aodp);
    temp1 = ck2 / pl;
    temp2 = temp1 / pl;
    cosio = cos(xincl);
    sinio = sin(xincl);
    sin2u = sin(2.0 * u);
    cos2u = cos(2.0 * u);
    theta2 = cosio * cosio;
    x3thm1 = 3.0 * theta2 - 1.0;
    x1mth2 = 1.0 - theta2;
    x7thm1 = 7.0 * theta2 - 1.0;
    r = (rk - 0.5 * temp1 * x1mth2 * cos2u) ...
        / (1.0 - 1.5 * temp2 * betal * x3thm1);
    u = uk + 0.25 * temp2 * x7thm1 * sin2u;
    xnodeo = xnodek - 1.5 * temp2 * cosio * sin2u;
    xincl = xinck - 1.5 * temp2 * cosio * sinio * cos2u;
    rdot = rdotk + xn * temp1 * x1mth2 * sin2u;
    rfdot = rfdotk - xn * temp1 * (x1mth2 * cos2u + 1.5 * x3thm1);
    temp = r * rfdot / xke;
    pl = temp * temp;
    temp = 2.0 / r - (rdot * rdot + rfdot * rfdot) / (xke * xke);
    aodp = 1.0 / temp;
    xn = xke * aodp ^ (-1.5);
    if (abs(a2 - pl) < 1.0d-13)
        
        break
        
    end
    if (i == 20)
        
        fprintf('\n20 iterations in first loop');
        
    end
    
end
% the next values are calculated from constants and a combination of mean
% and intermediate quantities from the first loop. these values all remain
% fixed and are used in the second loop.
% preliminary values for the second loop
ecose = 1.0 - r / aodp;
esine = r * rdot / (xke * sqrt(aodp));
elsq = 1.0 - pl / aodp;
xlcof = 0.125 * a3ovk2 * sinio * (3.0 + 5.0 * cosio) / (1.0 + cosio);
aycof = 0.25 * a3ovk2 * sinio;
temp1 = esine / (1.0 + sqrt(1.0 - elsq));
cosu = cos(u);
sinu = sin(u);
% the second loop normally converges in about six iterations to the final
% mean value for the eccentricity, eo. the mean perigee, omegao, is also
% determined. cosepw and sinepw are found to twelve decimal places and
% are used to calculate an intermediate value for the eccentric anomaly,
% temp2. temp2 is then used in kepler's equation to find an intermediate
% value for the true longitude, capu.
% seed values for loop
eo = sqrt(elsq);
omegao = wk;
axn = eo * cos(omegao);
for i = 1:1:20
    a2 = eo;
    beta = 1.0 - eo * eo;
    temp = 1.0 / (aodp * beta);
    aynl = temp * aycof;
    ayn = eo * sin(omegao) + aynl;
    cosepw = r * cosu / aodp + axn - ayn * temp1;
    sinepw = r * sinu / aodp + ayn + axn * temp1;
    axn = cosepw * ecose + sinepw * esine;
    ayn = sinepw * ecose - cosepw * esine;
    omegao = mod(atan2(ayn - aynl, axn), pi2);
    if (eo > 0.5)
        
        eo = 0.9 * eo + 0.1 * (axn / cos(omegao));
        
    else
        
        eo = axn / cos(omegao);
        
    end
    if (eo > 0.999)
        
        eo = 0.999;
        
    end
    if (abs(a2 - eo) < 1.0d-9)
        
        break
        
    end
    if (i == 20)
        
        fprintf('\n20 iterations in second loop');
        
    end
    
end
temp2 = atan2(sinepw, cosepw);
capu = temp2 - esine;
xll = temp * xlcof * axn;
% xll adjusts the intermediate true longitude
% capu, to the mean true longitude, xl
xl = capu - xll;
xmo = mod(xl - omegao, pi2);
% the third loop usually converges after three iterations to the
% mean semi-major axis, a1, which is then used to find the mean motion, xno
a0 = aodp;
a1 = a0;
beta2 = sqrt(beta);
temp = 1.5 * ck2 * x3thm1 / (beta * beta2);
for i = 1:1:20
    a2 = a1;
     = temp / (a0 * a0);
    a0 = aodp * (1.0 - );
    d1 = temp / (a1 * a1);
    a1 = a0 / (1.0 - d1 / 3.0 - d1 * d1 - 134.0 * d1 * d1 * d1 / 81.0);
    if (abs(a2 - a1) < 1.0e-13)
        
        break
        
    end
    if (i == 20)
        
        fprintf('\n20 iterations in third loop');
        
    end
end
xno = xke * a1 ^ (-1.5);
xno = xno / (pi2 / 1440.0);
