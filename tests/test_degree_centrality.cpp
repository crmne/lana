#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include "degree_centrality.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE DegreeCentrality test
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

struct Node {
    unsigned int degree;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & degree;
    }
};


template <typename Graph>
struct GraphFixture {
    typedef typename property_map<Graph, unsigned int Node::*>::type VertexUIntMap;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

    GraphFixture() : g() {
        vector<Vertex> v;
        static const size_t size = 5;

        cm = get(&Node::degree, g);

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
    VertexUIntMap cm;
};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, undirectedS, Node> Graph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node> Bigraph;

typedef boost::mpl::list<Graph> AllDegreeCentralitiesTypes;
typedef boost::mpl::list<Digraph, Bigraph> GregariousnessTypes;
typedef boost::mpl::list<Bigraph> PopularityTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(AllDegreeCentralities, G, AllDegreeCentralitiesTypes)
{
    graph_traits<Graph>::vertex_iterator it, end;
    unsigned int i, results[] = { 3, 2, 2, 1, 2 };

    GraphFixture<G> f;

    all_degree_centralities(f.g, f.cm);

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.cm[*it], results[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Gregariousness, G, GregariousnessTypes)
{
    graph_traits<Graph>::vertex_iterator it, end;
    unsigned int i, results[] = { 1, 1, 1, 1, 1 };

    GraphFixture<G> f;

    all_gregariousness_values(f.g, f.cm);

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.cm[*it], results[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popularity, G, PopularityTypes)
{
    graph_traits<Graph>::vertex_iterator it, end;
    unsigned int i, results[] = { 2, 1, 1, 0, 1 };

    GraphFixture<G> f;

    all_popularity_values(f.g, f.cm);

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.cm[*it], results[i]);
    }
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
