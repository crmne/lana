#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/compressed_sparse_row_graph.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include "benchmark.hpp"

#include <fstream>
#include "graph_loaders.hpp"

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

void print_log(const char *algorithm, const char *graph_type, benchmark::event_list &events, bool root)
{
    long microseconds = events.average_time().total_microseconds();
    boost::posix_time::time_duration avg = boost::posix_time::microseconds(average(microseconds));

    std::vector<long> all_results = gather_all(microseconds);
    // print all results log
    {

        if (root) {
            cout << "# Wall clock time for each process" << endl;
            cout << "# format: algorithm graph_type process time microseconds" << endl;
            size_t j = 0;

            for (std::vector<long>::iterator i = all_results.begin(); i != all_results.end(); ++i, ++j) {
                boost::posix_time::time_duration time = boost::posix_time::microseconds(*i);
                cout << algorithm << " " << graph_type << " " << j << " " << time << " " << *i << endl;
            }

            cout << endl << endl;
        }
    }

    // print the average
    {
        if (root) {
            cout << "# Average wall clock time" << endl;
            cout << "# format: algorithm graph_type nprocs average_time microseconds" << endl;
            cout << algorithm << " " << graph_type << " " << all_results.size() << " " << avg << " " << avg.total_microseconds() << endl;
            cout << endl << endl;
        }
    }
}
