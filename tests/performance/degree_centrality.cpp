#include "utils.hpp"
#include <boost/graph/metis.hpp>
#include "graph_loaders.hpp"
#include "degree_centrality.hpp"

namespace Csv
{
    struct Node {
        Node() {}
        Node(const std::string& name) : name(name) {}
        std::string name;
        unsigned int degree;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & name & degree;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef typename property_map<Graph, unsigned int Node::*>::type VertexUIntMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexUIntMap dcm;

    public:
        Fixture() {
            g = new Graph();

            if (is_root()) {
                ifstream in("graph.csv");
                sigsna::load_graph_from_csv(in, graph());
            }

            dcm = get(&Node::degree, graph());
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

        VertexUIntMap& degree_centrality_map() {
            return dcm;
        }
    };
}

namespace Metis
{
    struct Node {
        unsigned int degree;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & degree;
        }
    };

    template <typename Graph>
    class Fixture
    {

        typedef typename property_map<Graph, unsigned int Node::*>::type VertexUIntMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexUIntMap dcm;

    public:
        Fixture() {
            ifstream in("graph.metis");
            graph::metis_reader reader(in);
            g = new Graph(reader.begin(), reader.end(), reader.num_vertices());
            dcm = get(&Node::degree, graph());
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

        VertexUIntMap& degree_centrality_map() {
            return dcm;
        }
    };

};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, Metis::Node> MetisDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, Metis::Node> MetisBigraph;
typedef compressed_sparse_row_graph<directedS, Metis::Node, no_property, no_property, distributedS<mpi_process_group> > MetisCSRDigraph;
DECLARE_TYPE_NAME(MetisDigraph);
DECLARE_TYPE_NAME(MetisBigraph);
DECLARE_TYPE_NAME(MetisCSRDigraph);

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
typedef compressed_sparse_row_graph<directedS, Csv::Node, no_property, no_property, distributedS<mpi_process_group> > CsvCSRDigraph;
DECLARE_TYPE_NAME(CsvDigraph);
DECLARE_TYPE_NAME(CsvBigraph);
DECLARE_TYPE_NAME(CsvCSRDigraph);

typedef mpl::list<CsvDigraph, CsvBigraph> CsvAllDegreeGraphTypes;
typedef mpl::list<CsvDigraph, CsvBigraph> CsvOutDegreeGraphTypes;
typedef mpl::list<CsvBigraph> CsvInDegreeGraphTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(CsvAllDegreeScalabilityTest, G, CsvAllDegreeGraphTypes)
{
    benchmark::event_list events;
    Csv::Fixture<G> f;

    BENCHMARK(events, all_degree_centralities(f.graph(), f.degree_centrality_map()));

    write_all_results_log("AllDegree", GET_TYPE_NAME(f.graph()), events, f.is_root(), false);
    write_average_log("AllDegree", GET_TYPE_NAME(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CsvOutDegreeScalabilityTest, G, CsvOutDegreeGraphTypes)
{
    benchmark::event_list events;
    Csv::Fixture<G> f;

    BENCHMARK(events, all_out_degree_values(f.graph(), f.degree_centrality_map()));

    write_all_results_log("OutDegree", GET_TYPE_NAME(f.graph()), events, f.is_root(), false);
    write_average_log("OutDegree", GET_TYPE_NAME(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CsvInDegreeScalabilityTest, G, CsvInDegreeGraphTypes)
{
    benchmark::event_list events;
    Csv::Fixture<G> f;

    BENCHMARK(events, all_in_degree_values(f.graph(), f.degree_centrality_map()));

    write_all_results_log("InDegree", GET_TYPE_NAME(f.graph()), events, f.is_root(), false);
    write_average_log("InDegree", GET_TYPE_NAME(f.graph()), events, f.is_root());
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
