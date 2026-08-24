#include "../include/SPHSolver.h"
#include <eigen3/Eigen/src/Core/Matrix.h>

float SPHSolver::ComputeKernel(float r){
    float sigma = 8.0f / ((float)M_PI * h_ * h_ * h_);
    float q = r/h_;
    if( q <= 0.5f && q >= 0.0f){
        return sigma * (6.0f * (q*q*q - q*q) + 1.0f);
    }
    else if(q <= 1.0f && q >= 0.5f){
        return sigma * 2.0f * (1.0f- q)*(1.0f- q)*(1.0f- q);
    }
    else{
        return 0;
    }
}

void SPHSolver::ComputeDensity(){
    std::size_t N = particles_.size();
    std::vector<std::size_t> neighbors;
    #pragma omp parallel for
    for (int i = 0; i<N; i++){
        if(particles_[i].isboundary){
        particles_[i].density = rho0_;  
        continue;
    }
        float rhoi = 0.0f;
        for (auto& j : hasher.GetNeighbors(particles_[i])){
            float distance = (particles_[j].pos - particles_[i].pos).norm();
            if(distance <= h_){
                float Wij = ComputeKernel(distance);
                Eigen::Vector3f gradWij = ComputeKernelGradient(particles_[i].pos, particles_[j].pos);
                rhoi += particles_[j].mass * Wij + dt_ * (particles_[i].vel - particles_[j].vel).dot(gradWij);
            }
        }
        particles_[i].density = rhoi;
    }
}

Eigen::Vector3f SPHSolver::ComputeKernelGradient(const Eigen::Vector3f xi, const Eigen::Vector3f xj){
    Eigen::Vector3f rij = xi-xj;
    float r = rij.norm();
    if (r > h_ || r == 0.0f){
        return Eigen::Vector3f::Zero();
    }
    float sigma = 8.0f / ((float)M_PI * h_ * h_ * h_);
    float q = r / h_;
    float dW_dq;
    if (q <= 0.5f){
        dW_dq = sigma * 6.0f * (3.0f*q*q - 2.0f*q);
    }
    else{
        dW_dq = sigma * -6.0f * (1.0f- q)*(1.0f- q);
    }
    return 1/h_ * dW_dq * (rij/r);
}

void SPHSolver::UpdateVelocity1(){
    std::size_t N = particles_.size();
    #pragma omp parallel for
    for(int i =0; i < N; i++){
        if(particles_[i].isboundary) continue;
        float mi = particles_[i].mass;
        particles_[i].vel += dt_ * (ComputeViscosity(i) + gravity_);
        }
}

Eigen::Vector3f SPHSolver::ComputeViscosity(std::size_t i){
    std::size_t N = particles_.size();
    Eigen::Vector3f xi = particles_[i].pos;
    Eigen::Vector3f vi = particles_[i].vel;
    Eigen::Vector3f viscosity = Eigen::Vector3f::Zero();
    for (auto& j : hasher.GetNeighbors(particles_[i])) {
        Eigen::Vector3f rij = xi - particles_[j].pos;
        float r = rij.norm();
        if (r < 1e-6f || r > h_) continue;
        if (particles_[j].density < 1e-6f) continue; 
        Eigen::Vector3f vij = vi - particles_[j].vel;
        Eigen::Vector3f gradW = ComputeKernelGradient(xi, particles_[j].pos);
        float denom = r * r + 0.01f * h_ * h_;
        viscosity += (particles_[j].mass / particles_[j].density)
                     * vij * (gradW.dot(rij) / denom);
    }
    return 2.0f * nu_ * viscosity;
}

void SPHSolver::ComputePressure(){
    float k_tait = rho0_ * cs_ * cs_ / 7.0f;
    for (auto& p : particles_) {
        float ratio  = p.density / rho0_;
        float ratio7 = ratio * ratio * ratio * ratio * ratio * ratio * ratio;
        p.pressure   = k_tait * (ratio7 - 1.0f);
        if (p.pressure < 0.0f) p.pressure = 0.0f;
    }
}

Eigen::Vector3f SPHSolver::ComputePressureForce(std::size_t i){
    std::size_t N = particles_.size();
    Eigen::Vector3f xi = particles_[i].pos;
    float pi = particles_[i].pressure;
    float rhoi = particles_[i].density;
    Eigen::Vector3f pressure_force = Eigen::Vector3f::Zero();
    for (auto& j : hasher.GetNeighbors(particles_[i])){
        Eigen::Vector3f xj = particles_[j].pos;
        float pj = particles_[j].pressure;
        float rhoj = particles_[j].density;
        float mj = particles_[j].mass;
        Eigen::Vector3f grad_Wij = ComputeKernelGradient(xi, xj);
        pressure_force += mj * grad_Wij * (pi/(rhoi * rhoi)+pj/(rhoj * rhoj));
    }
    return pressure_force;
}

void SPHSolver::UpdateVelocity2(){
    std::size_t N = particles_.size();
    #pragma omp parallel for
    for(int i =0; i < N; i++){
        if(particles_[i].isboundary) continue;
        float mi = particles_[i].mass;
        particles_[i].vel -= dt_ * ComputePressureForce(i);
        }
}

void SPHSolver::UpdatePosition(){
    std::size_t N = particles_.size();
    for(int i =0; i < N; i++){
            if(particles_[i].isboundary) continue;
            particles_[i].pos += dt_ * particles_[i].vel;
        }
}

void SPHSolver::ApplyBoundaryConditions(){
    for(auto& p : particles_){
        if(p.pos[0] < xmin_){
            p.pos[0] = xmin_ + 0.0001f;
            p.vel[0] *= -0.2;
        }
        if(p.pos[0] > xmax_){
            p.pos[0] = xmax_ - 0.0001f;
            p.vel[0] *= -0.2;
        }
        if(p.pos[1] < ymin_){
            p.pos[1] = ymin_ + 0.0001f;
            p.vel[1] *= -0.2;
        }
        if(p.pos[1] > ymax_){
            p.pos[1] = ymax_ - 0.0001f;
            p.vel[1] *= -0.2;
        }
        if(p.pos[2] < zmin_){
            p.pos[2] = zmin_ + 0.0001f; 
            p.vel[2] *= -0.2;
        }
        if(p.pos[2] > zmax_){
            p.pos[2] = zmax_ - 0.0001f;
            p.vel[2] *= -0.2;
        }
    }
}

void SPHSolver::BuildHasher(){
    hasher.Build(particles_);
}