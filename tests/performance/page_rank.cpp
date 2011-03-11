#include "utils.hpp"
#include "graph_loaders.hpp"
#include <boost/graph/distributed/page_rank.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/linear_congruential.hpp>

#ifndef __ALGORITHM_NAME__
#define __ALGORITHM_NAME__ "PageRank"
#endif

namespace Csv
{
    struct Node {
        Node() {}
        Node(const std::string& name) : name(name) {}
        std::string name;
        double pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & name & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef typename property_map<Graph, double Node::*>::type VertexDoubleMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexDoubleMap prm;

    public:
        Fixture(const char* graphfile) {
            g = new Graph();

            if (is_root()) {
                ifstream in(graphfile);
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

        VertexDoubleMap& pagerank_map() {
            return prm;
        }
    };
}

namespace SmallWorld
{
    struct Node {
        double pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef small_world_iterator<minstd_rand, Graph> SWGen;
        typedef typename property_map<Graph, double Node::*>::type VertexDoubleMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;

        Graph *g;
        VertexDoubleMap prm;

    public:
        Fixture(const unsigned int nodes, const unsigned int nearest, const float prob) {
            boost::minstd_rand gen(1); // Generate the same graph
            g = new Graph(SWGen(gen, nodes, nearest, prob), SWGen(), nodes);
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

        VertexDoubleMap& pagerank_map() {
            return prm;
        }
    };

};

namespace ErdosRenyi
{
    struct Node {
        double pagerank;
        template<typename Archiver>
        void serialize(Archiver &ar, const unsigned int) {
            ar & pagerank;
        }
    };

    template <typename Graph>
    class Fixture
    {
        typedef erdos_renyi_iterator<minstd_rand, Graph> ERGen;
        typedef typename property_map<Graph, double Node::*>::type VertexDoubleMap;
        typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
        typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
        typedef typename graph_traits<Graph>::edges_size_type edges_size_type;

        Graph *g;
        VertexDoubleMap prm;

    public:
        Fixture(vertices_size_type nodes, edges_size_type edges) {
            boost::minstd_rand gen(1); // Generate the same graph
            g = new Graph(ERGen(gen, nodes, edges), ERGen(), nodes);
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

        VertexDoubleMap& pagerank_map() {
            return prm;
        }
    };

};

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, SmallWorld::Node> SmallWorldDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, SmallWorld::Node> SmallWorldBigraph;
DECLARE_TYPE_NAME(SmallWorldDigraph);
DECLARE_TYPE_NAME(SmallWorldBigraph);

typedef mpl::list<SmallWorldDigraph, SmallWorldBigraph> SmallWorldGraphTypes; // PageRank doesn't work with CSR graphs

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTest1, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(28250, 10, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTest2, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(28250, 100, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTestMed1, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(282500, 10, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTestMed2, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(282500, 100, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTestBig, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(653646, 86, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SmallWorldScalabilityTestHuge, G, SmallWorldGraphTypes)
{
    benchmark::event_list events;
    SmallWorld::Fixture<G> f(1307292, 86, 0.05);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, directedS, ErdosRenyi::Node> ErdosRenyiDigraph;
typedef adjacency_list <vecS, distributedS<mpi_process_group, vecS>, bidirectionalS, ErdosRenyi::Node> ErdosRenyiBigraph;
DECLARE_TYPE_NAME(ErdosRenyiDigraph);
DECLARE_TYPE_NAME(ErdosRenyiBigraph);

typedef mpl::list<ErdosRenyiDigraph, ErdosRenyiBigraph> ErdosRenyiGraphTypes;

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTest1, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(28250, 141250);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTest2, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(28250, 1412500);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTestMed1, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(282500, 1412500);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTestMed2, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(282500, 14125000);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTestBig, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(653646, 27811816);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ErdosRenyiScalabilityTestHuge, G, ErdosRenyiGraphTypes)
{
    benchmark::event_list events;
    ErdosRenyi::Fixture<G> f(1307292, 55623632);
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

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
    Csv::Fixture<G> f("graph.csv");
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CsvScalabilityTestBig, G, CsvGraphTypes)
{
    benchmark::event_list events;
    Csv::Fixture<G> f("subscriptions.csv");
    BENCHMARK(events, page_rank(f.graph(), f.pagerank_map()));
    write_average_log(__ALGORITHM_NAME__, GET_TYPE_NAME(f.graph()), total_vertices(f.graph()), total_edges(f.graph()), events, f.is_root());
}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
#pragma pomp inst init
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
