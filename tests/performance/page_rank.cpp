#include "utils.hpp"
#include <boost/graph/metis.hpp>
#include "graph_loaders.hpp"
#include <boost/graph/distributed/page_rank.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/random/linear_congruential.hpp>

namespace Csv
{
    struct Node {
        Node() {}
        Node(const std::string& name) : name(name) {}
        std::string name;
        float pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & name & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef typename property_map<Graph, float Node::*>::type VertexFloatMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexFloatMap prm;

    public:
        Fixture() {
            g = new Graph();

            if (is_root()) {
                ifstream in("graph.csv");
                sigsna::load_graph_from_csv(in, graph());
            }

            prm = get(&Node::pagerank, graph());
            synchronize(process_group(graph()));
        }

        virtual ~Fixture() {
            delete g;
        }

        bool is_root() {
            return (process_id(process_group(graph())) == 0);
        }

        Graph& graph() {
            return *g;
        }

        VertexFloatMap& pagerank_map() {
            return prm;
        }
    };
}

namespace SmallWorld
{
    struct Node {
        float pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef small_world_iterator<minstd_rand, Graph> SWGen;
        typedef typename property_map<Graph, float Node::*>::type VertexFloatMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexFloatMap prm;

    public:
        Fixture() {
            boost::minstd_rand gen;
            const unsigned int nodes = 28250, nearest = 10;
            const float prob = 0.05;
            g = new Graph(SWGen(gen, nodes, nearest, prob), SWGen(), nodes);
            prm = get(&Node::pagerank, graph());
        }

        virtual ~Fixture() {
            delete g;
        }

        bool is_root() {
            return (process_id(process_group(graph())) == 0);
        }

        Graph& graph() {
            return *g;
        }

        VertexFloatMap& pagerank_map() {
            return prm;
        }
    };

};

namespace Metis
{
    struct Node {
        float pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {

        typedef typename property_map<Graph, float Node::*>::type VertexFloatMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexFloatMap prm;

    public:
        Fixture() {
            ifstream in("graph.metis");
            graph::metis_reader reader(in);
            g = new Graph(reader.begin(), reader.end(), reader.num_vertices());
            prm = get(&Node::pagerank, graph());
        }

        virtual ~Fixture() {
            delete g;
        }

        bool is_root() {
            return (process_id(process_group(graph())) == 0);
        }

        Graph& graph() {
            return *g;
        }

        VertexFloatMap& pagerank_map() {
            return prm;
        }
    };

};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, SmallWorld::Node> SmallWorldDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, SmallWorld::Node> SmallWorldBigraph;
DECLARE_TYPE_NAME(SmallWorldDigraph);
DECLARE_TYPE_NAME(SmallWorldBigraph);

typedef mpl::list<SmallWorldDigraph, SmallWorldBigraph> SmallWorldGraphTypes; // PageRank doesn't work with CSR graphs

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTest, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f;

    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));

    // write_all_results_log("PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root(), false);
    write_average_log("PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root());
}

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Metis::Node> MetisDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Metis::Node> MetisBigraph;
DECLARE_TYPE_NAME(MetisDigraph);
DECLARE_TYPE_NAME(MetisBigraph);

typedef mpl::list<MetisDigraph, MetisBigraph> MetisGraphTypes;

// too bad it doesn't work
// BOOST_AUTO_TEST_CASE_TEMPLATE(MetisScalabilityTest, G, MetisGraphTypes)
// {
//     benchmark::event_list events;
//     Metis::Fixture<G> f;

//     BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));

//     log_average_values(std::cout, "PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root());
// }

// Enable named vertex support
namespace boost
{
    namespace graph
    {
        template<>
        struct internal_vertex_name<Csv::Node> {
            typedef multi_index::member<Csv::Node, std::string, &Csv::Node::name> type;
        };

        template<>
        struct internal_vertex_constructor<Csv::Node> {
            typedef vertex_from_name<Csv::Node> type;
        };
    }
}

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Csv::Node> CsvDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Csv::Node> CsvBigraph;
DECLARE_TYPE_NAME(CsvDigraph);
DECLARE_TYPE_NAME(CsvBigraph);

typedef mpl::list<CsvDigraph, CsvBigraph> CsvGraphTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(CsvScalabilityTest, G, CsvGraphTypes)
{
    benchmark::event_list events;
    Csv::Fixture<G> f;

    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));

    // write_all_results_log("PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root(), false);
    write_average_log("PageRank", GET_TYPE_NAME(f.graph()), events, f.is_root());
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
#pragma pomp inst init
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
