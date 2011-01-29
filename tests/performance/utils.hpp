#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/compressed_sparse_row_graph.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include "benchmark.hpp"

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE PageRank test
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

// Type printing utils
#define DECLARE_TYPE_NAME(x) template<> const char *type_name<x>::name = #x;
#define GET_TYPE_NAME(x) (type_name<typeof(x)>::name)
template <typename T> class type_name
{
public:
    static const char *name;
};

// MPI utils
template<typename T>
T average(T result)
{
    T sum = 0;
    mpi::communicator world;
    mpi::reduce(world, result, sum, std::plus<T>(), 0);
    return sum / world.size();
}

template<typename T>
std::vector<T> gather_all(T result)
{
    std::vector<T> results;
    mpi::communicator world;
    mpi::gather(world, result, results, 0);
    return results;
}

void log_all_results_values(std::ostream& out, const char *algorithm, const char *graph_type, benchmark::event_list &events, bool root)
{
    unsigned long long microseconds = events.average_time().total_microseconds();
    std::vector<unsigned long long> all_results = gather_all(microseconds);

    if (root) {
        size_t j = 0;

        for (std::vector<unsigned long long>::iterator i = all_results.begin(); i != all_results.end(); ++i, ++j) {
            boost::posix_time::time_duration time = boost::posix_time::microseconds(*i);
            out << algorithm << " " << graph_type << " " << j << " " << time << " " << *i / 1000000.0  << endl;
        }
    }
}

void log_all_results_header(std::ostream &out, bool root)
{
    if (root) {
        out << "# Wall clock time for each process" << endl;
        out << "# Algorithm GraphType Process AverageTimeString AverageTimeSeconds" << endl;
    }
}

void log_average_values(std::ostream& out, const char *algorithm, const char *graph_type, benchmark::event_list &events, bool root)
{
    unsigned long long microseconds = events.average_time().total_microseconds();
    boost::posix_time::time_duration avg = boost::posix_time::microseconds(average(microseconds));

    if (root) {
        mpi::communicator world;
        out << algorithm << " " << graph_type << " " << world.size() << " " << avg << " " << avg.total_microseconds() / 1000000.0  << endl;
    }
}

void log_average_header(std::ostream& out, bool root)
{
    if (root) {
        out << "# Average wall clock time" << endl;
        out << "# Algorithm GraphType Processes AverageTimeString AverageTimeSeconds" << endl;
    }
}

void write_average_log(const char *algorithm, const char *graph_type, benchmark::event_list &events, bool root, bool declare = true)
{
    std::ostringstream filename;
    filename << algorithm << "-" << graph_type << "-Average.log"; // Why not sprintf? I hate buffer overflows.
    std::ostream *of;
    mpi::communicator world;

    if (world.size() == 1) {
        if (root) {
            of = new std::ofstream(filename.str().c_str());
        } else {
            of = &std::cout;
        }

        log_average_header(*of, root);
    } else {
        if (root) {
            of = new std::ofstream(filename.str().c_str(), ios_base::app);
        } else {
            of = &std::cout;
        }
    }

    log_average_values(*of, algorithm, graph_type, events, root);

    if (root && declare) {
        std::cout << "Wrote " << filename.str() << std::endl;
        delete of;
    }
}

void write_all_results_log(const char *algorithm, const char *graph_type, benchmark::event_list &events, bool root, bool declare = true)
{
    mpi::communicator world;
    std::ostringstream filename;
    filename << algorithm << "-" << graph_type << "-" << world.size() << "Processes.log";
    std::ostream *of;

    if (root) {
        of = new std::ofstream(filename.str().c_str());
    } else {
        of = &std::cout;
    }

    log_all_results_header(*of, root);
    log_all_results_values(*of, algorithm, graph_type, events, root);

    if (root && declare) {
        std::cout << "Wrote " << filename.str() << std::endl;
        delete of;
    }
}
