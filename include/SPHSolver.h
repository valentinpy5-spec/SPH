#ifndef SPHSOLVER_H_
#define SPHSOLVER_H_


#include "SpatialHash.h"
#include <cstddef>
#include <eigen3/Eigen/Dense>



class SPHSolver{
    public: 

    SPHSolver(const std::vector<Particle>& particles,float h, float rho0,
    float cs, float dt, float nu, float xmin, float xmax, float ymin, float ymax,
    float zmin, float zmax, Eigen::Vector3f gravity ): particles_(particles), hasher(particles.size(),h), h_(h),rho0_(rho0),
    cs_(cs),dt_(dt), nu_(nu), xmin_(xmin), xmax_(xmax), ymin_(ymin),ymax_(ymax),
    zmin_(zmin), zmax_(zmax), gravity_(gravity) {}

    void ComputeDensity();
    void ComputePressure();
    void UpdateVelocity1();
    void UpdateVelocity2();   
    void UpdatePosition();
    void ApplyBoundaryConditions();
    void BuildHasher();

    const std::vector<Particle>& particles() const {return particles_;}

    private:
    std::vector<Particle> particles_;
    SpatialHash hasher;
    const float h_;
    const float rho0_;
    const float cs_;        
    const float dt_;
    const float nu_;
    const float xmin_;
    const float xmax_;
    const float ymin_;
    const float ymax_;
    const float zmin_;
    const float zmax_;


    Eigen::Vector3f gravity_;
    

    float ComputeKernel(float r);
    Eigen::Vector3f ComputePressureForce(std::size_t i);
    Eigen::Vector3f ComputeViscosity(std::size_t i);
    Eigen::Vector3f ComputeKernelGradient(const Eigen::Vector3f xi, const Eigen::Vector3f xj );
    
};


















#endif