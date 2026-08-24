#include "../include/SimulationParameters.h"

#include <cassert>
#include <fstream>

#include <iostream>
#include <jsoncpp/json/json.h>


SimulationParameters::SimulationParameters(
    float dt_seconds, float duration_seconds,
    float h, float rho0, float cs, float nu,
    float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
    const std::string& input_file,
    const std::string& output_file_name_pattern)
    : dt_seconds_(dt_seconds),
      duration_seconds_(duration_seconds),
      h_(h),
      rho0_(rho0),
      cs_(cs),
      nu_(nu),
      xmin_(xmin),
      xmax_(xmax),
      ymin_(ymin),
      ymax_(ymax),
      zmin_(zmin),
      zmax_(zmax),
      input_file_(input_file),
      output_file_name_pattern_(output_file_name_pattern) {}

SimulationParameters::SimulationParameters(const SimulationParameters& other)
    : dt_seconds_(other.dt_seconds_),
      duration_seconds_(other.duration_seconds_),
      h_(other.h_),
      rho0_(other.rho0_),
      cs_(other.cs_),
      nu_(other.nu_),
      xmin_(other.xmin_),
      xmax_(other.xmax_),
      ymin_(other.ymin_),
      ymax_(other.ymax_),
      zmin_(other.zmin_),
      zmax_(other.zmax_),
      input_file_(other.input_file_),
      output_file_name_pattern_(other.output_file_name_pattern_) {
  assert(false);
}

SimulationParameters SimulationParameters::CreateFromJsonFile(
    const std::string& input_file_path) {
  std::ifstream in(input_file_path, std::ios::in);

  Json::Reader json_reader;
  Json::Value json_root;

  bool read_succeeded = json_reader.parse(in, json_root);
  assert(read_succeeded);

  float dt_seconds = json_root.get("dt", 1.0 / 300.0).asFloat();
  float duration_seconds = json_root.get("total_time", 1.0).asFloat();

  double xmin = json_root["domain"][0].asFloat();
  double xmax = json_root["domain"][1].asFloat();
  double ymin = json_root["domain"][2].asFloat();
  double ymax = json_root["domain"][3].asFloat();
  double zmin = json_root["domain"][4].asFloat();
  double zmax = json_root["domain"][5].asFloat();
  double h = json_root.get("h", 1.0).asFloat();
  float rho0 = json_root.get("rho0", 1.0).asFloat();
  float cs = json_root.get("cs", 1.0).asFloat();
  float nu = json_root.get("nu", 1.0).asFloat();

  std::string input_file = json_root["particles"].asString();
  std::string output_file_name_pattern =
      json_root.get("output_fname", std::string("output.%04d.txt")).asString();

  return SimulationParameters(dt_seconds, duration_seconds, h, rho0,cs,nu,xmin, xmax, ymin, ymax, zmin, zmax, input_file,
                              output_file_name_pattern);
}

SimulationParameters::~SimulationParameters() {}

SimulationParameters ReadSimulationParameters(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "ERROR: .json file argument not found!" << std::endl;
    std::cout << "Usage: ./FluidSimulator [.json file path]" << std::endl;
    assert(false); 
  }

  return SimulationParameters::CreateFromJsonFile(argv[1]);
}