#include <boost/mpi/communicator.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>

using namespace boost;

template<typename Graph>
const size_t total_vertices(Graph &graph)
{
    size_t sum = 0;
    mpi::communicator world;
    mpi::reduce(world, num_vertices(graph), sum, std::plus<size_t>(), 0);
    return sum;
}

template<typename Graph>
const size_t total_edges(Graph &graph)
{
    size_t sum = 0;
    mpi::communicator world;
    mpi::reduce(world, num_edges(graph), sum, std::plus<size_t>(), 0);
    return sum;
}
