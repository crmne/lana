#include "utils.hpp"
#include <boost/graph/distributed/page_rank.hpp>

struct Node {
    Node() {}
    Node(const std::string& name) : name(name) {}
    float pagerank;
    std::string name;
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & name & pagerank;
    }
};

// Enable named vertex support
namespace boost
{
    namespace graph
    {
        template<>
        struct internal_vertex_name<Node> {
            typedef multi_index::member<Node, std::string, &Node::name> type;
        };

        template<>
        struct internal_vertex_constructor<Node> {
            typedef vertex_from_name<Node> type;
        };
    }
}

template <typename Graph>
struct GraphFixture {
    typedef typename property_map<Graph, float Node::*>::type VertexFloatMap;
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

    GraphFixture(): g() {
        prm = get(&Node::pagerank, g);

        if (is_root()) {
            istream *in = new ifstream("/Users/carmine/Desktop/itnet.csv");
            sigsna::load_graph_from_csv(*in, g);
        }

        synchronize(g.process_group());
    }

    bool is_root() {
        return (process_id(g.process_group()) == 0);
    }

    Graph g;
    VertexFloatMap prm;
};


typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Node> Digraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Node> Bigraph;
// typedef compressed_sparse_row_graph<directedS, Node, no_property, no_property, distributedS<mpi_process_group> > CSRDigraph;

DECLARE_TYPE_NAME(Digraph);
DECLARE_TYPE_NAME(Bigraph);

typedef mpl::list<Digraph, Bigraph> GraphTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(ScalabilityTest, G, GraphTypes)
{
    benchmark::event_list events;
    GraphFixture<G> f;

    for (size_t i = 0; i < 3; ++i) {
        BENCHMARK(events, page_rank(f.g, f.prm));
    }

    print_log("PageRank", GET_TYPE_NAME(f.g), events, f.is_root());
}


int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
