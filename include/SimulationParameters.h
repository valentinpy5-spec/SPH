#ifndef SIMULATION_PARAMETERS_H_
#define SIMULATION_PARAMETERS_H_

#include <eigen3/Eigen/Dense>
#include <string>

class SimulationParameters {
 public:

  SimulationParameters(float dt_seconds, float duration_seconds,
                       float h, float rho0, float cs, float nu, float xmin,
                       float xmax, float ymin, float ymax, float zmin, float zmax,
                       const std::string& input_file, const std::string& output_file_name_pattern);


  SimulationParameters(const SimulationParameters& other);

  // Returns a new set of configuration settings for a simulation read from a
  // .json file.
  static SimulationParameters CreateFromJsonFile(
      const std::string& input_file_path);

  ~SimulationParameters();

  float dt_seconds() const { return dt_seconds_; }
  float duration_seconds() const { return duration_seconds_; }
  float xmin() const { return xmin_; }
  float xmax() const { return xmax_; }
  float ymin() const { return ymin_; }
  float ymax() const { return ymax_; }
  float zmin() const { return zmin_; }
  float zmax() const { return zmax_; }
  float h() const {return h_;}
  float rho0() const {return rho0_;}
  float cs() const {return cs_;}
  float nu() const {return nu_;}
  const std::string& input_file() const { return input_file_; }
  const std::string& output_file_name_pattern() const {
    return output_file_name_pattern_;
  }


 private:
  SimulationParameters& operator=(const SimulationParameters& other);

  const float dt_seconds_;

  const float duration_seconds_;
  const float h_;
  const float rho0_; 
  const float cs_;
  const float nu_;
  const float xmin_;
  const float xmax_;
  const float ymin_;
  const float ymax_;
  const float zmin_;
  const float zmax_;


  const std::string input_file_;

  const std::string output_file_name_pattern_;
};

SimulationParameters ReadSimulationParameters(int argc, char** argv);

#endif  // SIMULATION_PARAMETERS_H_