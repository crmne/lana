#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/page_rank.hpp>

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
    double pagerank;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & pagerank;
    }
};

template <typename Graph>
struct GraphFixture {
    typedef typename property_map<Graph, double Node::*>::type VertexDoubleMap;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

    GraphFixture() : g() {
        vector<Vertex> v;
        static const size_t size = 5;

        prm = get(&Node::pagerank, g);

        if (process_id(g.process_group()) == 0) {
            // add vertices
            for (size_t i = 0; i < size; ++i) {
                v.push_back(add_vertex(g));
            }

            // add edges
            add_edge(v[0], v[1], g);
            add_edge(v[1], v[2], g);
            add_edge(v[2], v[0], g);
            add_edge(v[3], v[4], g);
            add_edge(v[4], v[0], g);
        }

        synchronize(g.process_group());
    }

    Graph g;
    VertexDoubleMap prm;
};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node> Bigraph;

typedef mpl::list<Digraph, Bigraph> PageRankTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(PageRank, G, PageRankTypes)
{
    typedef typename graph_traits<G>::vertex_iterator VertexIterator;
    double results[] = { 1.6112730806608355, 1.51958211856171, 1.4416448007774534, 0.15000000000000002, 0.27750000000000002 };

    GraphFixture<G> f;

    page_rank(f.g, f.prm, graph::n_iterations(1000));

    VertexIterator it, end;
    unsigned int i;

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.prm[*it], results[i]);
    }
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
