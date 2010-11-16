#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "csv.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE CSV test
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(ReadTest)
{
    std::string data1[2][3];
    data1[0][0] = "1";
    data1[0][1] = "2";
    data1[0][2] = "4";
    data1[1][0] = "2.91";
    data1[1][1] = "9.23";
    data1[1][2] = "4.9242395";
    std::string str1 = "1,2,4\n2.91,9.23,4.9242395";

    std::istringstream iss1(str1, std::istringstream::in);
    CSV csv1(iss1);

    for (size_t i = 0; i < 2; ++i) {
        std::vector<std::string> ret1 = csv1.nextRow();

        for (size_t j = 0; j < 3; ++j) {
            BOOST_CHECK_EQUAL(ret1[j], data1[i][j]);
        }
    }
}
