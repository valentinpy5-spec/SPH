#pragma once

#include <cstddef>
#include <vector>
#include <cstdint>
#include "Particle.h"

static const uint32_t P1 = 73856093;
static const uint32_t P2 = 19349663;
static const uint32_t P3 = 83492791;



struct Cell{
    int x,y,z;
};


struct SpatialHash{
    std::vector<uint32_t> cell_count;
    std::vector<uint32_t> cell_start;
    std::vector<uint32_t> sorted_indices;
    std::vector<uint32_t> temp_count;
    uint32_t size;
    float h; 
    
    SpatialHash(uint32_t particles, float h_) : h(h_), size(2*particles) {
        cell_count.assign(size, 0);
        cell_start.assign(size + 1, 0); 
        sorted_indices.resize(particles);
        temp_count.assign(size,0);
    };

    Cell GetCell(const Eigen::Vector3f& pos){
    Cell cell = {(int) std::floor(pos[0]/h), (int) std::floor(pos[1]/h), (int) std::floor(pos[2]/h)};
    return cell;
    };

    uint32_t HashCell(Cell cell){
        uint32_t hash = ((cell.x * P1)^(cell.y * P2)^(cell.z * P3)) % size;
        return hash;
    };


    void Build(const std::vector<Particle>& particles) {
            uint32_t N = particles.size();

            std::fill(cell_count.begin(), cell_count.end(), 0);
            for (int i = 0; i < N; i++) {
                Cell c = GetCell(particles[i].pos);
                cell_count[HashCell(c)]++;
            }

            cell_start[0] = 0;
            for (uint32_t k = 0; k < size; k++)
                cell_start[k + 1] = cell_start[k] + cell_count[k];


            std::fill(temp_count.begin(), temp_count.end(), 0);
            for (int i = 0; i < N; i++) {
                Cell c = GetCell(particles[i].pos);
                uint32_t key = HashCell(c);
                sorted_indices[cell_start[key] + temp_count[key]] = i;
                temp_count[key]++;
            }
        };
    
    std::vector<std::size_t> GetNeighbors(const Particle& particle){
        Eigen::Vector3f center = particle.pos;
        Cell c = GetCell(center);
        std::vector<std::size_t> neighbors;

        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                for(int k = -1; k <= 1; k++){
                    Cell neighbor = {c.x + i, c.y + j, c.z + k};
                    uint32_t key = HashCell(neighbor);
                    for (uint32_t s = cell_start[key]; s < cell_start[key+1]; s++){
                        std::size_t l = (std::size_t) sorted_indices[s];
                        neighbors.push_back(l);
                    }
                }
            }
        }
        return neighbors;

    }
    
};