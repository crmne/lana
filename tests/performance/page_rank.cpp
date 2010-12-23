#include "utils.hpp"
#include <boost/graph/metis.hpp>
#include <boost/graph/distributed/page_rank.hpp>

struct Node {
    float pagerank;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & pagerank;
    }
};


template <typename Graph>
class CSVFixture
{
public:
    CSVFixture() {
    }
    virtual ~CSVFixture() {
    }
};

template <typename Graph>
class METISFixture
{
    typedef typename property_map<Graph, float Node::*>::type VertexFloatMap;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

    Graph *g;
    VertexFloatMap prm;

public:
    METISFixture() {
        ifstream in("/Users/carmine/Desktop/itnet.metis");
        graph::metis_reader reader(in);
        g = new Graph(reader.begin(), reader.end(), reader.num_vertices());
        prm = get(&Node::pagerank, graph());
    }

    virtual ~METISFixture() {
        delete g;
    }

    bool is_root() {
        return (process_id(graph().process_group()) == 0);
    }

    Graph& graph() {
        return *g;
    }

    VertexFloatMap& pagerank_map() {
        return prm;
    }
};


typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node> Bigraph;
typedef compressed_sparse_row_graph<directedS, Node, no_property, no_property, distributedS<mpi_process_group> > CSRDigraph;

DECLARE_TYPE_NAME(Digraph);
DECLARE_TYPE_NAME(Bigraph);
DECLARE_TYPE_NAME(CSRDigraph);

typedef mpl::list<Digraph, Bigraph> GraphTypes; // PageRank doesn't accept CSR graphs

BOOST_AUTO_TEST_CASE_TEMPLATE(ScalabilityTest, G, GraphTypes)
{
    benchmark::event_list events;
    METISFixture<G> f;

    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));

    log_average_values(std::cout, "PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root());
}


int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
