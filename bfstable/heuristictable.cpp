#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

#include "heuristictable.hpp"
#include "bfs.hpp"

namespace Heuristic
{
    std::vector<uint8_t> twistSliceTable(TWIST_SLICE_SIZE, UNVISITED);
    std::vector<uint8_t> flipSliceTable(FLIP_SLICE_SIZE, UNVISITED);
    std::vector<uint8_t> cpSliceTable(CP_SLICE_SIZE, UNVISITED);
    std::vector<uint8_t> udEdgeSliceTable(UD_EDGE_SLICE_SIZE, UNVISITED);
}


void heuristictable::loadTables()
{
    loadTable("tables/twist_slice.bin", Heuristic::twistSliceTable);
    loadTable("tables/flip_slice.bin", Heuristic::flipSliceTable);
    loadTable("tables/cp_slice.bin", Heuristic::cpSliceTable);
    loadTable("tables/udedge_slice.bin", Heuristic::udEdgeSliceTable);
}


void heuristictable::generateTables()
{
    // Generate pruning tables
    std::cout<<"generating tables\n";
    Bfs bf;
    bf.bfstwstsls(Heuristic::twistSliceTable);
    bf.bfsflpsls(Heuristic::flipSliceTable);
    bf.bfscpsls(Heuristic::cpSliceTable);
    // TODO:
    // bf cpslice
    // bf udedge_slice

    saveTable("tables/twist_slice.bin", Heuristic::twistSliceTable);
    saveTable("tables/flip_slice.bin", Heuristic::flipSliceTable);
    saveTable("tables/cp_slice.bin", Heuristic::cpSliceTable);
    saveTable("tables/udedge_slice.bin", Heuristic::udEdgeSliceTable);
}


void heuristictable::saveTable(
    const std::string& filename,
    const std::vector<uint8_t>& table)
{
    std::ofstream out(filename, std::ios::binary);

    if (!out)
        throw std::runtime_error("Could not open table file for writing: " + filename);

    out.write(
        reinterpret_cast<const char*>(table.data()),
        table.size()
    );

    if (!out)
        throw std::runtime_error("Error writing table: " + filename);
}


void heuristictable::loadTable(const std::string& filename, std::vector<uint8_t>& table)
{
    std::ifstream in(filename, std::ios::binary);

    if (!in)
        throw std::runtime_error("Could not open table file for reading: " + filename);

    in.read(
        reinterpret_cast<char*>(table.data()),
        table.size()
    );

    if (!in)
        throw std::runtime_error("Error reading table: " + filename);
}


void heuristictable::load_or_generatetable()
{
    namespace fs = std::filesystem;

    const std::string directory = "tables";

    fs::create_directories(directory);

    const bool tablesExist =
        fs::exists(directory + "/twist_slice.bin") &&
        fs::exists(directory + "/flip_slice.bin") &&
        fs::exists(directory + "/cp_slice.bin") &&
        fs::exists(directory + "/udedge_slice.bin");

    if (tablesExist)
    {
        std::cout << "Loading pruning tables...\n";

        loadTables();

        std::cout << "Tables loaded.\n";
    }
    else
    {
        std::cout << "Pruning tables not found.\n";
        std::cout << "Generating tables...\n";

        generateTables();

        std::cout << "Tables generated and saved.\n";
    }
}