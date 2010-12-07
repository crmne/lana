#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>

#include <boost/graph/distributed/page_rank.hpp>
#include "benchmark.hpp"

#include <boost/random.hpp>
// #include <boost/graph/rmat_graph_generator.hpp> // BUG: apparently including this breaks the unit_test_framework
#include <boost/graph/small_world_generator.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE PageRank test
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

struct Node {
    float pagerank;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & pagerank;
    }
};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node> Bigraph;

typedef mpl::list<Digraph, Bigraph> GraphTypes;

// BOOST_AUTO_TEST_CASE_TEMPLATE(RMAT, G, GraphTypes)
// {
//     minstd_rand gen;
//     const double a = 0.5, b = 0.25, c = 0.1, d = 0.15;
//     const size_t vertices_number = 1000, edges_number = 1000;
//     G g(rmat_iterator<minstd_rand, G>(gen, vertices_number, edges_number, a, b, c, d),
//         rmat_iterator<minstd_rand, G>(), vertices_number);

//     typedef typename property_map<G, float Node::*>::type VertexFloatMap;
//     VertexFloatMap prm = get(&Node::pagerank, g);
//     BENCHMARK(page_rank(g, prm));
// }

// Sorted RMAT
// Sorted unique RMAT
// Unique RMAT
// Scalable RMAT

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorld, G, GraphTypes)
{
    minstd_rand gen;
    const double probability = 0.03;
    const size_t vertices_number = 1000, neighbors = 10;
    G g(small_world_iterator<minstd_rand, G>(gen, vertices_number, neighbors, probability),
        small_world_iterator<minstd_rand, G>(), vertices_number);

    typedef typename property_map<G, float Node::*>::type VertexFloatMap;
    VertexFloatMap prm = get(&Node::pagerank, g);
    BENCHMARK(page_rank(g, prm));
}

// Erdos-Renyi
// Sorted Erdos-Renyi
// SSCA
// Mesh

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
