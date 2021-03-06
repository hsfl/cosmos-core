#include "sph_sim.h"

// Evaluates the smoothing kernel function for the SPH equations
MatrixXd kernel(MatrixXd r, MatrixXd h, MatrixXd type) {
	MatrixXd s = r.array() / h.array();
	
	MatrixXd W(s.rows(), s.cols());

	// Cubic spline kernel, used for vehicle-reduced density (type 1) particle interactions.
	//W = W + (type==1).*( ( 1 - 3/2*s.^2        + 3/4*s.^3 )          .*( s<1 ) + ( 1/4*(2-s).^3 )           .*( (s >= 1)   .*(s <= 2) ) )   ./(pi*h.^3);
	//W = W + (type==1)*( ( 1.0-3.0/2.0*pow(s,2.0) + 3.0/4.0*pow(s,3.0) ) * (s<1) + ( 1.0/4.0*pow((2.0-s),3.0) ) * ( (s >= 1.0) * (s <= 2.0) ) ) / M_PI*pow(h,3.0);

	// Quadratic kernel, used for all other (type 2) interactions
	//W = W + ( type==2 ).*15./(16*pi*h.^3)     .*(s.^2/4-s+1)         .*(s<2);
	//W = W + (type==1)*15.0/(16.0*M_PI*pow(h,3.0))*(pow(s,2.0),4.0-s+1.0)*(s<2.0);
	return W;
}
// Evaluates the smoothing kernel function for the SPH equations
double kernel(double r, double h, int type) {
	double s = r/h;
	
	double W = 0;

	// Cubic spline kernel, used for vehicle-reduced density (type 1) particle interactions.
	//W = W + (type==1).*( ( 1 - 3/2*s.^2        + 3/4*s.^3 )          .*( s<1 ) + ( 1/4*(2-s).^3 )           .*( (s >= 1)   .*(s <= 2) ) )   ./(pi*h.^3);
	W = W + (type==1)*( ( 1.0-3.0/2.0*pow(s,2.0) + 3.0/4.0*pow(s,3.0) ) * (s<1) + ( 1.0/4.0*pow((2.0-s),3.0) ) * ( (s >= 1.0) * (s <= 2.0) ) ) / M_PI*pow(h,3.0);

	// Quadratic kernel, used for all other (type 2) interactions
	//W = W + ( type==2 ).*15./(16*pi*h.^3)     .*(s.^2/4-s+1)         .*(s<2);
	W = W + (type==1)*15.0/(16.0*M_PI*pow(h,3.0))*(pow(s,2.0),4.0-s+1.0)*(s<2.0);
	return W;
}

// Evaluates the derivatie of the smoothing kernel function for the SPH equations.
// Note that this returns a scalar value which is the magnitude of the gradient W.
// The direction (if needed) must be computed separately.
double kernel_grad(double r, double h, int type) {
	double s = r/h;

	double dWdr = 0;

	// Cubic spline kernel, used for vehicle-reduced density (type 1) particle interactions.
	//dWdr = dWdr + ( type==1 ).*( ( -3*s + 9/4*s.^2 )     .*(s<1) + ( -3/4*(2-s).^2 )           .*( (s >= 1).*(s <= 2) ) )./(pi*h.^4);
	dWdr = dWdr + (type==1)*( ( -3.0*s + 9.0/4.0*pow(s,2.0))*(s<1) + ( -3.0/4.0*pow((2.0-s),2.0) )*( (s>=1.0)*(s<=2.0)  ) )/(M_PI*pow(h,4.0));

	// Quadratic kernel, used for all other (type 2) interactions
	//dWdr = dWdr + ( type==2 ).*15./(16*pi*h.^4)     .*(s/2-1)   .*(s<2);
	dWdr = dWdr + (type==2)*15.0/(16.0*M_PI*pow(h,4.0))*(s/2.0-1.0)*(s<2.0);

	return dWdr;
}

// Return vector of indices of nonzero elements. As row if A is a row vector, as a column vector otherwise
MatrixXi find(const MatrixXd& A) {
	MatrixXi idxs(1,A.size());
	int size = 0;
	for(Index i=0; i<A.size(); ++i) {
		if(A(i)) {
			idxs(0,size) = i;
			size ++;
		}
	}
	if(size > 0) {
		idxs.conservativeResize(1,size);
		if(A.rows() > 1) {
			idxs.transposeInPlace();
		}
	} else {
		idxs.resize(0,0);
	}
	
	return idxs;
}

// Return a matrix whose elements are those indices of m specified by I. Necessarily, returned matrix will be the same shape as I
// Iow: indexing the elements of m with another matrix I
MatrixXd index(const MatrixXd& m, const MatrixXi& I) {
	MatrixXd out(I.rows(),I.cols());

	for(int i = 0; i < I.size(); ++i) {
		out(i) = m(I(i));
	}

	return out;
}

// Assign a double value to the indices of m specified in the matrix I
MatrixXd assign_d_by_index(MatrixXd& m, const MatrixXi& I, const double& s) {
	for(int i = 0; i < I.size(); ++i) {
		m(I(i)) = s;
	}
	return m;
}

// Returns a row vector of doubles in rising sequence by 1 (eg: {0, 1, 2, 3, ... }), inclusive
MatrixXd vseq(int val0, int val_last) {
	if(val_last < val0) {
		throw "valn must be greater than val0!";
	}
	MatrixXd out(1,val_last-val0+1);
	for(int i = val0; i <= val_last; ++i) {
		out(i) = i;
	}
	return out;
}

// Return sorted indices of c, in ascending order
MatrixXi sort(const MatrixXd& c) {
	MatrixXi idxs(1,c.size());
	for(int i = 0; i < c.size(); ++i) {
		idxs(i) = i;
	}
	sort(idxs.data(), idxs.data()+idxs.size(),[&](int i, int j)
          { return c(i) < c(j);});
	return idxs;
}

// Append another matrix app to matrix m, right or down
void append_right(MatrixXd& m, const MatrixXd& app) {
	int off = m.cols();
	m.conservativeResize(m.rows(),off + app.cols());
	m(seq(0,last),seq(off,last)) = app;
}
void append_down(MatrixXd& m, const MatrixXd& app) {
	int off = m.rows();
	m.conservativeResize(off + app.rows(), m.cols());
	m(seq(off,last),seq(0,last)) = app;
}



sph_sim::sph_sim(param_struct param, group_conf_struct group_conf, double t0 /*= 0*/) {
	sph_sim::param = param;
	sph_sim::group_conf = group_conf;
	sph_sim::t0 = t0;

	// Setup SPH properties
	init_prop();
	compute_hij();
	kernel_type();

	// Initialize positions and velocities
	init_states();
	
}

void sph_sim::init_prop() {
	int N = 0;

	// Initialize vehicles
	for(int i = 0; i < this->group_conf.num_veh.size(); ++i) {
		for(int j = 0; j < this->group_conf.num_veh[i]; ++j) {
			// Motion constraints
			this->prop.vmin(N,0) = this->group_conf.veh_limits.vmin(i);
			this->prop.vmax(N,0) = this->group_conf.veh_limits.vmax(i);
			this->prop.turning_radius(N,0) = this->group_conf.veh_limits.turning_radius(i);
			this->prop.amax(N,0) = pow(this->prop.vmax(N),2.0) / this->prop.turning_radius(N);

			// Smoothing width
			this->prop.h(N,0) = this->group_conf.veh_h(i);

			// Mass
			this->prop.m(N,0) = rho0 / kernel(0, this->prop.h(N), 2);

			// Kernel values at 0 and h
			double KER0 = kernel(0, this->prop.h(N), 2);
			double KERh = kernel(this->prop.h(N), this->prop.h(N), 2);

			// Kernel gradient at h
			double KERG = kernel_grad(this->prop.h(N), this->prop.h(N), 2);

			// Pressure force ~ K*Fp
			double Fp = rho0 * KERh / KER0;
			// Viscous force ~ mu*Fmu
			double Fmu = 2.0 * this->prop.vmax(N) * KER0 * KERG / ( rho0 * this->prop.h(N) * pow((KER0+KERh),2.0) );

			// Force coeficients found by solving:
			//	amax = vmax^2/turning_radius = K*Fp + mu*Fmu
			//	Re = (K*Fp)/(mu*Fmu)
			//
			// This enforces the desired Reynolds number at r_ij=h
			// and limits the acceleration magnitude to be on the
			// order of amax.
			this->prop.mu(N,0) = this->prop.vmax(N) / this->prop.turning_radius(N) / Fmu / (1.0 + this->param.Re);
			this->prop.K(N,0) = this->param.accel.veh * this->param.Re * this->prop.mu(N) * Fmu / Fp;

			// Group number and particle type
			this->prop.group(N,0) = i;
			this->prop.particle_type(N,0) = particle_type_enum::veh;	// NOTE: may produce oob error?
			N++;
		}
	}

	this->nveh = N;

	// Obstacles
	for(int i = 0; i < this->group_conf.num_obs; ++i) {
		// Motion constraints
		this->prop.vmin(N,0) = 0;
		this->prop.vmax(N,0) = 0;
		this->prop.turning_radius(N,0) = 0;
		this->prop.amax(N,0) = 0;

		// Smoothing width
		this->prop.h(N,0) = this->group_conf.obs_h(i);

		// Mass
		this->prop.m(N,0) = 2.0 * rho0 / kernel(0, this->prop.h(N), 2);

		// Kernel values at 0 and h
		double KER0 = kernel(0, this->prop.h(N), 2);
		double KERh = kernel(this->prop.h(N), this->prop.h(N), 2);

		// Kernel gradient at h
		double KERG = kernel_grad(this->prop.h(N), this->prop.h(N), 2);

		// Force coeficients:
		this->prop.mu(N,0) = 0;
		this->prop.K(N,0) = this->param.accel.obs * this->prop.amax.maxCoeff() * KER0 / (rho0 * KERh);

		// Group number and particle type
		this->prop.group(N,0) = i;
		this->prop.particle_type(N,0) = particle_type_enum::obs;
		N++;
	}

	this->nobs = this->group_conf.num_obs;

	// Reduced density particles
	for(int i = 0; i < this->group_conf.num_rd; ++i) {
		// Motion constraints
		this->prop.vmin(N,0) = 0;
		this->prop.vmax(N,0) = 0;
		this->prop.turning_radius(N,0) = 0;
		this->prop.amax(N,0) = 0;

		// Smoothing width
		this->prop.h(N,0) = this->group_conf.rd_h(i);

		// Mass
		this->prop.m(N,0) = rho0 / kernel(0, this->prop.h(N), 1) * 1e-8;

		// Force coeficients:
		// No viscosity for attractors
		this->prop.mu(N,0) = 0;
		// NOTE: check bounds on the seqs (seq(0,this->nveh))
		int gr = this->prop.group(seq(0,this->nveh-1),0).rows(); // NOTE: Check here, was causing errors, see also computer_hij
		int gc = this->prop.group(seq(0,this->nveh-1),0).cols();
		MatrixXi I = find( ( this->prop.group(seq(0,this->nveh-1),0).array() == this->group_conf.rd_group(i) )
													.select(MatrixXd::Ones(gr,gc), MatrixXd::Zero(gr,gc)) );
		this->prop.K(N,0) = -1.0 * this->param.accel.rd * index(this->prop.amax,I).maxCoeff()
							* kernel(0, this->prop.h(N),1) / kernel_grad(this->prop.h(N),this->prop.h(N), 1);

		// Group number and particle type
		this->prop.group(N,0) = i;
		this->prop.particle_type(N,0) = particle_type_enum::obs;
		N++;
	}
}

// Compute h_ij matrix
void sph_sim::compute_hij() { // NOTE: Check the this->prop.h(seq(I1,I2),0), same as above, was causing errors
	MatrixXd hi = this->prop.h(seq(0,this->nveh-1),0) * MatrixXd::Ones(1,this->nveh);
	MatrixXd hj = hi.transpose();

	// Vehicles: hij=max(hi,hj)
	this->prop.hij = ( hi.array() > hj.array() ).select(hi,hj);

	// Obstacles: hij=h_obs
	int I1 = this->nveh; // NOTE: +1 for 1 based indexing? Maybe not necessary. Consider I2 as well
	int I2 = this->nveh + this->group_conf.num_obs - 1;
	append_down(this->prop.hij, this->prop.h(seq(I1,I2),0) * MatrixXd::Ones(1,this->prop.hij.cols()));
	append_right(this->prop.hij, MatrixXd::Ones(this->prop.hij.rows(),1) * this->prop.h(seq(I1,I2),0).transpose());

	// Reduced density particles: hij=h_rd
	I1 = this->nveh + this->group_conf.num_obs;
	I2 = this->nveh + this->group_conf.num_obs + this->group_conf.num_rd - 1;
	append_down(this->prop.hij, this->prop.h(seq(I1,I2),0) * MatrixXd::Ones(1,this->prop.hij.cols()));
	append_right(this->prop.hij, MatrixXd::Ones(this->prop.hij.rows(),1) * this->prop.h(seq(I1,I2),0).transpose());
}

// Create a matrix kernel_type that tells which kernel to use.
// 1 is for vehicle-reduced density particle interactions,
// 2 is for all others
void sph_sim::kernel_type() {
	int N = this->prop.m.rows() > this->prop.m.cols() ? this->prop.m.rows() : this->prop.m.cols();

	MatrixXd ki = this->prop.particle_type * MatrixXd::Ones(1,N);
	MatrixXd kj = ki.transpose();

	this->prop.kernel_type = 2*MatrixXd::Ones(N,N);

	MatrixXd lhs = ( ki.array() == (int)particle_type_enum::veh*MatrixXd::Ones(N,N).array() ).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N));
	MatrixXd rhs = ( kj.array() == (int)particle_type_enum::rd*MatrixXd::Ones(N,N).array() ).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N));
	MatrixXi I = find( (lhs.array() != 0 && rhs.array() != 0).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N)) ); // NOTE: error here
	assign_d_by_index(this->prop.kernel_type, I, 1);
	lhs = ( kj.array() == (int)particle_type_enum::veh*MatrixXd::Ones(N,N).array() ).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N));
	rhs = ( ki.array() == (int)particle_type_enum::rd*MatrixXd::Ones(N,N).array() ).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N));
	I = find( (lhs.array() != 0 && rhs.array() != 0).select(MatrixXd::Ones(N,N), MatrixXd::Zero(N,N)) );
	assign_d_by_index(this->prop.kernel_type, I, 1);
}

// Set the initial SPH states (positions and velocities) for all particles
void sph_sim::init_states() {
	if(this->param.ndim == 2) {
		// 2D initialization
		this->init2d();
	} else {
		// 3D initialization
		this->init3d();
	}

	// Obstacles
	// NOTE: check bounds
	for(size_t i = 0; i < this->group_conf.num_obs-1; ++i) {
		MatrixXd app(1,6);
		app << this->group_conf.obs_init.x(i), this->group_conf.obs_init.y(i), this->group_conf.obs_init.z(i), 0, 0, 0;	// x,y,z, u,v,w
		append_down(this->states, app);
	}

	// Reduced density particles
	append_down(this->states, MatrixXd::Zero(this->group_conf.num_rd,6));

}

// 2D initialization, use a 2D hexagonal (close packed) lattice
void sph_sim::init2d() {
	// Basis vectors for a hexagonal lattice in 2D
	Vector3d v1(1, 0, 0);
	Vector3d v2(cos(M_PI/3.0), sin(M_PI/3.0), 0);

	int N = 90;
	// Row 1 in x-direction
	MatrixXd r(3,N);
	r << vseq(0,N-1), MatrixXd::Zero(2,N);
	int n = r.cols();
	for(int i = 1; i < N; ++i) {	// NOTE: Check that this is
		MatrixXd app(r.rows(), r.cols());
		if(i % 2 == 0) {
			app << r(0,all).array()+v2(0),
				   r(1,all).array()+v2(1),
				   r(2,all).array()+v2(2);
		} else {
			app << r(0,all).array()+v2(0)-v1(0),
				   r(1,all).array()+v2(1)-v1(1),
				   r(2,all).array()+v2(2)-v1(2);
		}
		append_right(r, app);
	}

	// Randomize slightly to avoid singularities
	r(seq(0,1),all) = r(seq(0,1),all).array() + MatrixXd::Random(2,r.cols()).array()/2*1e-8;

	// Shift to origin
	MatrixXd ave = r.rowwise().mean();
	r(0,all) = r(0,all).array()-ave(0);
	r(1,all) = r(0,all).array()-ave(1);
	r(2,all) = r(2,all).array()*0;

	// Sort r by distance from the origin
	// r is a 3xN matrix, with x y z for the rows
	MatrixXd d = r.array().pow(2).rowwise().sum().sqrt();
	MatrixXi I = sort(d);
	for(int i = 0; i < r.rows(); ++i) {
		r(i,all) = index(r(i,all), I);
	}

	// Shift to origin
	ave = r(all,0);
	r(0,all) = r(0,all).array()-ave(0);
	r(1,all) = r(0,all).array()-ave(1);
	r(2,all) = r(2,all).array()*0;

	// Sort r by distance from the origin
	d = r.array().pow(2).rowwise().sum().sqrt();
	I = sort(d);
	for(int i = 0; i < r.rows(); ++i) {
		r(i,all) = index(r(i,all), I);
	}

	// Shift to origin
	r(0,all) = r(0,all).array()-r(0,0);
	r(1,all) = r(0,all).array()-r(0,1);
	r(2,all) = r(2,all).array()*0;

	r = r.transpose();
	this->states.resize(0,0);

	// Set the initial positions
	for(int i = 0; i < this->group_conf.veh_init.x.size(); ++i) {
		int n;
		bool one_loop;
		if(this->group_conf.veh_init.x.size() < this->group_conf.num_veh.size()) {
			n = this->group_conf.num_veh.sum();
			one_loop = true;
		} else {
			n = this->group_conf.num_veh(i);
			one_loop = false;
		}

		double xmid = this->group_conf.veh_init.x(i);
		double ymid = this->group_conf.veh_init.y(i);
		double zmid = 0;

		double vx = this->group_conf.veh_init.u(i);
		double vy = this->group_conf.veh_init.v(i);
		double vz = 0;

		MatrixXd rtmp = r(all,seqN(1,3)) * 2 * this->group_conf.veh_h(i);
		for(auto row : rtmp.rowwise()) {
			row(3) = vx;
			row(4) = vy;
			row(5) = 0;
		}
		
		MatrixXd rn = rtmp(seqN(0,n),all);

		double xave = rn(all,0).mean();
		double yave = rn(all,1).mean();
		double zave = 0;

		rn(all,0) = rn(all,0).array()-xave+xmid;
		rn(all,1) = rn(all,1).array()-yave+xmid;
		rn(all,2) = rn(all,2)*0;

		
		this->states = MatrixXd(rn.rows()+this->states.rows(), rn.cols()); // NOTE: check this
		this->states << this->states, rn;

		if(one_loop) {
			break;
		}
	}
}

// 3D initialization, use a 3D hexagonal (close packed) lattice
void sph_sim::init3d() {
	// Basis vectors for a hexagonal lattice in 3D
	Vector3d v1(1, 0, 0);
	Vector3d v2(cos(M_PI/3.0), sin(M_PI/3.0), 0);
	// implement later
}

// Return the right hand side of the SPH momentum equation
void sph_sim::sph_rhs() {
	// Compute the interparticle distance and vectors
	MatrixXd dij;
	Matrix3D rij;
	Matrix3D unit_ij;
	tie(dij, rij, unit_ij) = sph_compute_dij();

	// Compute the mask function Mask(i,j) = 1 if particles interact
	MatrixXd Mask;
	MatrixXi MaskI;
	tie(Mask, MaskI) = sph_compute_mask(dij);

	// Compute density
	MatrixXd rho = sph_compute_density(dij, Mask, MaskI);

	// Remove diagonal elements from MaskI
	DiagonalMatrix<double, Dynamic> dm(this->npart);
	dm.diagonal() = VectorXd::Ones(this->npart);
	MatrixXd tmp = Mask - (MatrixXd)dm;
	MaskI = find( ( tmp.array() == 1 )
							.select(MatrixXd::Ones(tmp.rows(),tmp.cols()),
									MatrixXd::Zero(tmp.rows(),tmp.cols())) );
	
	// Compute gradW
	MatrixXd gradW = MatrixXd::Zero(this->npart, this->npart);
	// NOTE: INCOMPLETE
	//gradW(MaskI) = kernel_grad(dij(MaskI), this->prop.hij(MaskI), this->prop.kernel_type(MaskI));

	// Compute pressure
	MatrixXd P = sph_compute_pressure(rho);
	// NOTE: Check this
	MatrixXd P_term = (P.array() / rho.array().pow(2)).matrix() * this->prop.m.transpose() + MatrixXd::Ones(this->npart,1) * (P.array() * this->prop.m.array() / rho.array().pow(2)).transpose().matrix();
	// Magnitude of the pressure force
	P_term = P_term.array() * gradW.array();

	// Viscosity
	Matrix3D Pi_term = sph_compute_pi(rho, dij, rij, unit_ij, gradW, Mask, MaskI);

	Matrix3D DvDt;
	// NOTE: INCOMPLETE
	// DvDt = -cat ...
	// DvDt = ...

	// External forcing

}

// Compute the distance, vector, and unit vector between particles i and j
tuple<MatrixXd, Matrix3D, Matrix3D> sph_sim::sph_compute_dij() {
	// Create distance matrix for dij(i,j) = distance between particles i and j
	MatrixXd dx = this->states(all,0) * MatrixXd::Ones(1,this->npart);
	dx = dx-dx.transpose();
	MatrixXd dy = this->states(all,1) * MatrixXd::Ones(1,this->npart);
	dy = dy-dy.transpose();
	MatrixXd dz = this->states(all,2) * MatrixXd::Ones(1,this->npart);
	dz = dz-dz.transpose();

	MatrixXd dij = (dx.array().pow(2) + dy.array().pow(2) + dz.array().pow(2)).sqrt();

	Matrix3D rij(dx,dy,dz);
	Matrix3D unit_ij(dx.array()/dij.array(), dy.array()/dij.array(), dz.array()/dij.array());
	// Get rid of Infs generated by divisions by 0
	// NOTE: Check this
	MatrixXi I = find( unit_ij.z0.array().isInf().cast<double>() );
	assign_d_by_index(unit_ij.z0, I, 0);
	I = find( unit_ij.z1.array().isInf().cast<double>() );
	assign_d_by_index(unit_ij.z1, I, 0);
	I = find( unit_ij.z2.array().isInf().cast<double>() );
	assign_d_by_index(unit_ij.z2, I, 0);
	return make_tuple(dij, rij, unit_ij);
}

// Compute the masking function and the indices that are non-zero
//			{ 0 if dij>2*hij
// M(i,j) = { 0 if particle j is not a vehicle and group(i)~=group(j)
//			{ 0 if particle i is not a vehicle (except M(i,i)=1)
//			{ 1 else
tuple<MatrixXd, MatrixXi> sph_sim::sph_compute_mask(const MatrixXd& dij) {
	// NOTE: This function uses sparse matrices, but let's ignore that for now
	// Kernel is non-zero (i.e., dij < 2*hij)
	MatrixXd M = (dij(seqN(0,this->nveh),all).array() < 2*this->prop.hij(seqN(0,this->nveh),all).array())
					.select(MatrixXd::Ones(this->nveh,dij.cols()),MatrixXd::Zero(this->nveh,dij.cols()));
	
	// Obstacle or reduced density particle
	// NOTE: check that I'm understanding this correctly as an append down
	append_down(M, MatrixXd::Zero(this->npart,dij.cols()));			// M(i,:) = 0
	int n = this->nobs + this->nrd;
	DiagonalMatrix<double, Dynamic> dm(this->npart); // NOTE: check size of this, am I understanding this correctly
	dm.diagonal() = VectorXd::Ones(this->npart);
	M(seq(this->nveh,last),seq(this->nveh,last)) = (MatrixXd)dm;	// M(i,i) = 1

	// Reduced density particles
	for(int i = 0; i < this->nrd; ++i) {
		int I1 = this->nveh + this->nobs + i;
		MatrixXi I2 = find( ( this->prop.group.array() != this->prop.group(I1) )
								.select(MatrixXd::Ones(this->prop.group.rows(),this->prop.group.cols()),
										MatrixXd::Zero(this->prop.group.rows(),this->prop.group.cols())) );
		//M(I2,I1)=0; // NOTE: INCOMPLETE: what is this?
	}

	MatrixXi I = find( ( M.array() != 0 )
								.select(MatrixXd::Ones(M.rows(),M.cols()),
										MatrixXd::Zero(M.rows(),M.cols())) );
	
	return make_tuple(M,I);
}

// Mask I is a column vector of indices
MatrixXd sph_sim::sph_compute_density(const MatrixXd& dij, const MatrixXd& Mask, const MatrixXi& MaskI) {
	// Reshape mask vector into matrix
	MatrixXd mj = Mask * this->prop.m.transpose(); // NOTE: MODIFIED, what is the purpose of the .*(ones(obj.npart,1) ? Seems pointless
	
	// NOTE: Another sparse matrix
	MatrixXd K = MatrixXd::Zero(this->npart,this->npart);
	// NOTE: INCOMPLETE
	//assign_d_by_index(K, MaskI, kernel(index(dij,MaskI), index(this->prop.hij,MaskI), index(this->prop.kernel_type,MaskI)));
	MatrixXd rho = (mj.array()*K.array()).rowwise().sum();

	// Reduced density particles have fixed density that does not consider the proximity of other particles
	MatrixXd I = vseq(this->nveh+this->nobs, this->npart-1);
	// NOTE: INCOMPLETE
	//assign_d_by_index(rho, I, this->prop.m(I)*kernel(0,this->prop.h(I),2));
	
	return rho;
}

// Equation of state to compute the pressure
MatrixXd sph_sim::sph_compute_pressure(const MatrixXd& rho) {
	MatrixXd P = this->prop.K.array() * rho.array() * (rho0 - 1);
	return P;
}

// Compute the viscous forces
Matrix3D sph_sim::sph_compute_pi(const MatrixXd& rho, const MatrixXd& dij, const Matrix3D& rij, const Matrix3D& unit_ij,
								const MatrixXd& gradW, const MatrixXd& Mask, const MatrixXi& MaskI) {
	MatrixXd tmp = ( rho.array().pow(-1).matrix() * (2 * this->prop.m.array() / rho.array()).transpose().matrix() ).transpose().array() * gradW.array();
	// NOTE: INCOMPLETE
	// tmp(MaskI) = tmp(MaskI) ./ dij(MaskI);

	Matrix3D vji( MatrixXd::Ones(this->npart,1) * this->states(all,3).transpose() - this->states(all,3) * MatrixXd::Ones(1,this->npart),
				  MatrixXd::Ones(this->npart,1) * this->states(all,4).transpose() - this->states(all,4) * MatrixXd::Ones(1,this->npart),
				  MatrixXd::Ones(this->npart,1) * this->states(all,5).transpose() - this->states(all,5) * MatrixXd::Ones(1,this->npart) );
	
	// No viscosity for reduced density particles or obstacles
	vji.z0(all,seq(this->nveh,last)) = MatrixXd::Zero(vji.z0.rows(), vji.z0.cols()-this->nveh);	// NOTE: Check cols is correct on Zero matrix
	vji.z1(all,seq(this->nveh,last)) = MatrixXd::Zero(vji.z1.rows(), vji.z1.cols()-this->nveh);
	vji.z2(all,seq(this->nveh,last)) = MatrixXd::Zero(vji.z2.rows(), vji.z2.cols()-this->nveh);

	// NOTE: INCOMPLETE
	Matrix3D Pi;
	//Pi = -cat(... squeeze ...)
	return Pi;
}

// Compute the external force on vehicles to drive them toward a loiter circle
tuple<MatrixXd,MatrixXd,MatrixXd> sph_sim::external_force() {
	MatrixXd Fx = MatrixXd::Zero(this->states.rows(),1);
	MatrixXd Fy = Fx;
	MatrixXd Fz = Fx;
	for(int i = 0; i < this->group_conf.num_loiter; ++i) {
		int group_num = this->group_conf.loiter_group(i);
		MatrixXi II = find( ( this->prop.group.array() == group_num )
								.select(MatrixXd::Ones(this->prop.group.rows(),this->prop.group.cols()),
										MatrixXd::Zero(this->prop.group.rows(),this->prop.group.cols())) );
		
		// Loiter circle
		if(this->lR(i) > 0) {
			// Width of the "flat spot" in the potential field, controls the width of the loiter circle track
			double width = this->lR(i)/4;

			// Shift the center of the loiter circle
			//auto x = index(this->states,II)
		}
	}

	return make_tuple(Fx,Fy,Fz);
}

// TODO
// INCOMPLETE, MODIFIED