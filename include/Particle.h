#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <eigen3/Eigen/Dense>


struct Particle {
  Eigen::Vector3f pos;
  Eigen::Vector3f vel;
  float mass     = 0.0f;
  float density  = 1000.0f;   
  float pressure = 0.0f;
  float force    = 0.0f;
  bool isboundary = false;
};


#endif  // PARTICLE_H_