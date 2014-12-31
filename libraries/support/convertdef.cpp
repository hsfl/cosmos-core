#include "configCosmos.h"

#include "mathlib.h"
#include "jpleph.h"
#include "timelib.h"
#include "memlib.h"

//#include <sys/types.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>

#include <iostream>

using namespace std;


//std::ostream& operator << (std::ostream& out, const cartpos& a)
//{
//	out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
//	return out;
//}

//std::istream& operator >> (std::istream& in, cartpos& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const cposstruc& a)
//{
//	out << a.utc << "\t" << a.pos;
//	return out;
//}

//std::istream& operator >> (std::istream& in, cposstruc& a)
//{
//	in >> a.utc >> a.pos;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const geoidpos& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
//	return out;
//}

//std::istream& operator >> (std::istream& in, geoidpos& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const spherpos& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
//	return out;
//}

//std::istream& operator >> (std::istream& in, spherpos& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const aattstruc& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
//	return out;
//}

//std::istream& operator >> (std::istream& in, aattstruc& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const quatatt& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
//	return out;
//}

//std::istream& operator >> (std::istream& in, quatatt& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const dcmatt& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
//	return out;
//}

//std::istream& operator >> (std::istream& in, dcmatt& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const qatt& a)
//{
//	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
//	return out;
//}

//std::istream& operator >> (std::istream& in, qatt& a)
//{
//	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const kepstruc& a)
//{
//	out <<a.utc<<"\t"
//		<<a.orbit<<"\t"
//		<<a.period<<"\t"
//		<<a.a<<"\t"
//		<<a.e<<"\t"
//		<<a.h<<"\t"
//		<<a.beta<<"\t"
//		<<a.eta<<"\t"
//		<<a.i<<"\t"
//		<<a.raan<<"\t"
//		<<a.ap<<"\t"
//		<<a.alat<<"\t"
//		<<a.ma<<"\t"
//		<<a.ta<<"\t"
//		<<a.ea<<"\t"
//		<<a.mm<<"\t"
//		<<a.fa;
//	return out;
//}

//std::istream& operator >> (std::istream& in, kepstruc& a)
//{
//	in  >>a.utc
//		>>a.orbit
//		>>a.period
//		>>a.a
//		>>a.e
//		>>a.h
//		>>a.beta
//		>>a.eta
//		>>a.i
//		>>a.raan
//		>>a.ap
//		>>a.alat
//		>>a.ma
//		>>a.ta
//		>>a.ea
//		>>a.mm
//		>>a.fa;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const bodypos& a)
//{
//	out << a.sepangle << "\t" << a.size << "\t" << a.radiance;
//	return out;
//}

//std::istream& operator << (std::istream& in, bodypos& a)
//{
//	in >> a.sepangle >> a.size >> a.radiance;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const extrapos& a)
//{
//	out <<a.utc<<"\t"
//		<<a.tt<<"\t"
//		<<a.ut<<"\t"
//		<<a.tdb<<"\t"
//		<<a.j2e<<"\t"
//		<<a.dj2e<<"\t"
//		<<a.ddj2e<<"\t"
//		<<a.e2j<<"\t"
//		<<a.de2j<<"\t"
//		<<a.dde2j<<"\t"
//		<<a.j2t<<"\t"
//		<<a.j2s<<"\t"
//		<<a.t2j<<"\t"
//		<<a.s2j<<"\t"
//		<<a.s2t<<"\t"
//		<<a.ds2t<<"\t"
//		<<a.t2s<<"\t"
//		<<a.dt2s<<"\t"
//		<<a.sun2earth<<"\t"
//		<<a.sun2moon<<"\t"
//		<<a.closest;
//	return out;
//}

//std::istream& operator >> (std::istream& in, extrapos& a)
//{
//	in >>a.utc
//		>>a.tt
//		>>a.ut
//		>>a.tdb
//		>>a.j2e
//		>>a.dj2e
//		>>a.ddj2e
//		>>a.e2j
//		>>a.de2j
//		>>a.dde2j
//		>>a.j2t
//		>>a.j2s
//		>>a.t2j
//		>>a.s2j
//		>>a.s2t
//		>>a.ds2t
//		>>a.t2s
//		>>a.dt2s
//		>>a.sun2earth
//		>>a.sun2moon
//		>>a.closest;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const extraatt& a)
//{
//	out << a.utc << "\t" << a.j2b << "\t" << a.b2j;
//	return out;
//}

//std::istream& operator >> (std::istream& in, extraatt& a)
//{
//	in >> a.utc >> a.j2b >> a.b2j;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const posstruc& a)
//{
//	out << a.utc << "\t"
//		<< a.baryc << "\t"
//		<< a.eci << "\t"
//		<< a.sci << "\t"
//		<< a.geoc << "\t"
//		<< a.selc << "\t"
//		<< a.geod << "\t"
//		<< a.selg << "\t"
//		<< a.geos << "\t"
//		<< a.extra << "\t"
//		<< a.earthsep << "\t"
//		<< a.moonsep << "\t"
//		<< a.sunsize << "\t"
//		<< a.sunradiance;
//	return out;
//}

//std::istream& operator >> (std::istream& in, posstruc& a)
//{
//	in  >> a.utc
//		>> a.baryc
//		>> a.eci
//		>> a.sci
//		>> a.geoc
//		>> a.selc
//		>> a.geod
//		>> a.selg
//		>> a.geos
//		>> a.extra
//		>> a.earthsep
//		>> a.moonsep
//		>> a.sunsize
//		>> a.sunradiance;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const attstruc& a)
//{
//	out << a.utc << "\t"
//		<< a.topo << "\t"
//		<< a.lvlh << "\t"
//		<< a.geoc << "\t"
//		<< a.selc << "\t"
//		<< a.icrf << "\t"
//		<< a.extra;
//	return out;
//}

//std::istream& operator >> (std::istream& in, attstruc& a)
//{
//	in  >> a.utc
//		>> a.topo
//		>> a.lvlh
//		>> a.geoc
//		>> a.selc
//		>> a.icrf
//		>> a.extra;
//	return in;
//}

//std::ostream& operator << (std::ostream& out, const locstruc& a)
//{
//	out << a.utc << "\t"
//		<< a.pos << "\t"
//		<< a.att << "\t"
//		<< a.bearth;
//	return out;
//}

//std::istream& operator >> (std::istream& in, locstruc& a)
//{
//	in  >> a.utc
//		>> a.pos
//		>> a.att
//		>> a.bearth;
//	return in;
//}
