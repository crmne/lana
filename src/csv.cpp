#include "csv.hpp"

CSV::CSV(std::istream &in) : input(in) {}

// Reads the next row and puts it in the buffer
std::vector<std::string> CSV::nextRow()
{
    std::vector<std::string> result;
    std::string line;
    std::getline(input, line);

    std::stringstream line_stream(line);
    std::string cell;

    while (std::getline(line_stream, cell, ',')) {
        result.push_back(cell);
    }

    return result;
}
