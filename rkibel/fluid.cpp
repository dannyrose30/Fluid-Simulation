#include <iostream>
#include <vector>
#include <charconv>
#include <fstream>
#include <sstream>
#include <cmath>
#include "constants.hpp"

struct particle {
    const int id;
    double px;
    double py;
    double pz;
    double hvx;
    double hvy;
    double hvz;
    double vx;
    double vy;
    double vz;
};


float ppm;
int np;
double mass;
double smoothing_length;
std::vector<int> grid_size;
std::vector<double> block_size; 

int parseInt(char* arg) {
    const std::string input_str = arg;
    int res = 0;
    auto result = std::from_chars(input_str.data(), input_str.data() + input_str.size(), res);
    if (result.ec != std::errc()) {
        std::cerr << "Error: time steps must be numeric.\n";
        exit(1);
    }
    if (res < 0) {
        std::cerr << "Error: Invalid number of time steps.\n";
        exit(1);
    }
    return res;
}

std::vector<particle> parseInput(char* inputFile) {
    std::ifstream fileReader;
    fileReader.open(inputFile, std::ios::binary);
    if (!fileReader) {
        std::cerr << "Error: Cannot open " << inputFile << " for reading";
        exit(1);
    }
    fileReader.read(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    fileReader.read(reinterpret_cast<char*>(&np), sizeof(np));
    if (np <= 0) {
        std::cerr << "Error: Invalid number of particles: " << np << ".\n";
        exit(-5);
    }

    int counter = 0;
    std::vector<particle> particles;
    while (!fileReader.eof()) {
        float px, py, pz, hvx, hvy, hvz, vx, vy, vz;
        fileReader.read(reinterpret_cast<char*>(&px), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&py), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&pz), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&hvx), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&hvy), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&hvz), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&vx), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&vy), sizeof(float));
        fileReader.read(reinterpret_cast<char*>(&vz), sizeof(float));
        particles.push_back(particle {counter, px, py, pz, hvx, hvy, hvz, vx, vy, vz});
        counter++;
    }
    particles.pop_back();

    if (particles.size() != np) {
        std::cerr << "Error: Number of particles mismatch. Header: " << np << ", "
                     "Found: " << particles.size() << ".\n";
        exit(-5);
    }

    mass = constants::rho / ppm / ppm / ppm;
    smoothing_length = constants::r / ppm;
    grid_size = {static_cast<int>(std::floor((constants::xmax - constants::xmin) / smoothing_length)), 
                 static_cast<int>(std::floor((constants::ymax - constants::ymin) / smoothing_length)),
                 static_cast<int>(std::floor((constants::zmax - constants::zmin) / smoothing_length))};
    block_size = {(constants::xmax - constants::xmin) / grid_size[0], 
                  (constants::ymax - constants::ymin) / grid_size[1],
                  (constants::zmax - constants::zmin) / grid_size[2]};

    std::cout << "Number of particles: " << np << "\n"
            "Particles per meter: " << ppm << "\n"
            "Smoothing length: " << smoothing_length << "\n"
            "Particle mass: " << mass << "\n"
            "Grid size: " << grid_size[0] << " x " << grid_size[1] << " x " << grid_size[2] << "\n"
            "Number of blocks: " << grid_size[0] * grid_size[1] * grid_size[2] << "\n"
            "Block size: " << block_size[0] << " x " << block_size[1] << " x " << block_size[2] << "\n";

    return particles;
}

void testOutput(char* outputFile) {
    return;
}

void writeFile(std::string outputFile, float ppm, int np, std::vector<particle> particles) {
    std::ofstream file;
    file.open(outputFile, std::ios::binary);
    file.write(reinterpret_cast<char*>(&ppm), sizeof(ppm));
    file.write(reinterpret_cast<char*>(&np), sizeof(np));
    for (particle p: particles) {
        float px = p.px;
        float py = p.py;
        float pz = p.pz;
        float hvx = p.hvx;
        float hvy = p.hvy;
        float hvz = p.hvz;
        float vx = p.vx;
        float vy = p.vy;
        float vz = p.vz;
        file.write(reinterpret_cast<char*>(&px), sizeof(float));
        file.write(reinterpret_cast<char*>(&py), sizeof(float));
        file.write(reinterpret_cast<char*>(&pz), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvx), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvy), sizeof(float));
        file.write(reinterpret_cast<char*>(&hvz), sizeof(float));
        file.write(reinterpret_cast<char*>(&vx), sizeof(float));
        file.write(reinterpret_cast<char*>(&vy), sizeof(float));
        file.write(reinterpret_cast<char*>(&vz), sizeof(float));
    }
}


/*
run with
g++ -o fluid fluid.cpp
./fluid **timestep** **inputfile** **outputfile**
*/

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments: " << argc-1 << ".\n";
        return 1;
    }

    int nts = parseInt(argv[1]);
    std::vector<particle> particles = parseInput(argv[2]);
    testOutput(argv[3]);


    std::vector<particle> segmented;
    for (int i = 0; i < 4000; ++i) {
        segmented.push_back(particles[i]);
    }
    writeFile("test.fld", 0, 3000, segmented);

    return 0;
}