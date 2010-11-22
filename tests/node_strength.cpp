#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include "node_strength.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE NodeStrength test
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

struct Node {
    unsigned int strength;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & strength;
    }
};

struct Arc {
    unsigned int weight;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & weight;
    }
};

template <typename Graph>
struct GraphFixture {
    typedef typename property_map<Graph, unsigned int Node::*>::type VertexUIntMap;
    typedef typename property_map<Graph, unsigned int Arc::*>::type EdgeUIntMap;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename graph_traits<Graph>::edge_descriptor Edge;
    typedef typename graph_traits<Graph>::edge_iterator EdgeIterator;

    GraphFixture() : g() {
        vector<Vertex> v;
        static const size_t size = 5;

        sm = get(&Node::strength, g);
        ewm = get(&Arc::weight, g);

        if (process_id(g.process_group()) == 0) {
            size_t i;

            // add vertices
            for (i = 0; i < size; ++i) {
                v.push_back(add_vertex(g));
            }

            // add edges
            add_edge(v[0], v[1], g);
            add_edge(v[1], v[2], g);
            add_edge(v[2], v[0], g);
            add_edge(v[3], v[4], g);
            add_edge(v[4], v[0], g);

            // add weigths
            unsigned int weights[] = { 3, 2, 1, 7, 2 };

            EdgeIterator it, end;

            for (i = 0, tie(it, end) = edges(g); it != end; ++i, ++it) {
                ewm[*it] = weights[i];
            }
        }

        synchronize(g.process_group());
    }

    Graph g;
    VertexUIntMap sm;
    EdgeUIntMap ewm;
};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, undirectedS, Node, Arc> Graph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node, Arc> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node, Arc> Bigraph;

typedef mpl::list<Graph> AllNodeStrengthTypes;
typedef mpl::list<Digraph, Bigraph> OutStrengthTypes;
typedef mpl::list<Bigraph> InStrengthTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(AllNodeStrengths, G, AllNodeStrengthTypes)
{
    typedef typename graph_traits<G>::vertex_iterator VertexIterator;
    unsigned int i, results[] = { 3, 2, 1, 7, 2 };

    GraphFixture<G> f;

    all_node_strengths(f.g, f.sm, f.ewm);

    VertexIterator it, end;

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.sm[*it], results[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(OutStrength, G, OutStrengthTypes)
{
    typedef typename graph_traits<G>::vertex_iterator VertexIterator;
    unsigned int i, results[] = { 3, 2, 1, 7, 2 };

    GraphFixture<G> f;

    all_out_strength_values(f.g, f.sm, f.ewm);

    VertexIterator it, end;

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.sm[*it], results[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InStrength, G, InStrengthTypes)
{
    typedef typename graph_traits<G>::vertex_iterator VertexIterator;
    unsigned int i, results[] = { 3, 3, 2, 0, 7 };

    GraphFixture<G> f;

    all_in_strength_values(f.g, f.sm, f.ewm);

    VertexIterator it, end;

    for (i = 0, tie(it, end) = vertices(f.g); it != end; ++i, ++it) {
        BOOST_CHECK_EQUAL(f.sm[*it], results[i]);
    }
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
