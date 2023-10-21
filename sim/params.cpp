#include "params.hpp"

//#include "utility.cpp"

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
char * as_writable_buffer(T & value) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return reinterpret_cast<char *>(&value);
}

template <typename T>
requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
T read_binary_value(std::istream & is) {
  T value{};
  is.read(as_writable_buffer(value), sizeof(value));
  return value;
}

void params::initialize(std::istream& fileReader) {
    float read_ppm = read_binary_value<float>(fileReader);
    ppm = read_ppm;
    np = read_binary_value<int>(fileReader);
    if (np <= 0) {
        std::cerr << "Error: Invalid number of particles: " << np << ".\n";
        exit(-5);
    }
    mass = constants::fluid_density / ppm / ppm / ppm;
    smoothing_length = constants::radius_mult / ppm;
    density_factors = {
        smoothing_length*smoothing_length, 
        std::pow(smoothing_length, 6), 
        315.0 * mass / 64.0 / std::numbers::pi / std::pow(smoothing_length, 9)
    };
    acceleration_factors = {
        smoothing_length * smoothing_length,
        45.0 * mass * constants::stiff_pressure / std::numbers::pi / std::pow(smoothing_length, 6) / 2,
        45.0 * mass * constants::viscosity / std::numbers::pi / std::pow(smoothing_length, 6)
    };
    grid_size = {static_cast<int>(std::floor((constants::max[0] - constants::min[0]) / smoothing_length)), 
                 static_cast<int>(std::floor((constants::max[1] - constants::min[1]) / smoothing_length)),
                 static_cast<int>(std::floor((constants::max[2] - constants::min[2]) / smoothing_length))};
    block_size = {(constants::max[0] - constants::min[0]) / grid_size[0], 
                  (constants::max[1] - constants::min[1]) / grid_size[1],
                  (constants::max[2] - constants::min[2]) / grid_size[2]};
}