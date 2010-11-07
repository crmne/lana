#pragma once
#ifndef CSV_HPP_J0RYY2FH
#define CSV_HPP_J0RYY2FH

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

class CSV
{
public:
    CSV(std::istream &in);
    std::vector<std::string> nextRow();

private:
    std::istream &input;
};

#endif /* end of include guard: CSV_HPP_J0RYY2FH */
