#include <vector>
#include <iterator>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
// #include <boost/graph/distributed/compressed_sparse_row_graph.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/page_rank.hpp>
#include "benchmark.hpp"

#include <fstream>
#include "graph_loaders.hpp"

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

        if (i_am_the_root_proc()) {
            istream *in = new ifstream("/Users/carmine/Desktop/itnet.csv");
            sigsna::load_graph_from_csv(*in, g);
        }

        synchronize(g.process_group());
    }

    bool i_am_the_root_proc() {
        return (process_id(g.process_group()) == 0);
    }

    Graph g;
    VertexFloatMap prm;
};

template<typename T>
T average(T result)
{
    T sum = 0;
    mpi::communicator world;
    mpi::reduce(world, result, sum, std::plus<T>(), 0);
    return sum / world.size();
}

template<typename T>
std::vector<T> all(T result)
{
    std::vector<T> results;
    mpi::communicator world;
    mpi::gather(world, result, results, 0);
    return results;
}

#define DECLARE_TYPE_NAME(x) template<> const char *type_name<x>::name = #x;
#define GET_TYPE_NAME(x) (type_name<typeof(x)>::name)
template <typename T> class type_name
{
public:
    static const char *name;
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

    long microseconds = events.average_time().total_microseconds();
    boost::posix_time::time_duration avg = boost::posix_time::microseconds(average(microseconds));
    // print all results log
    {
        std::vector<long> all_results = all(microseconds);

        if (f.i_am_the_root_proc()) {
            cout << "# Wall clock time for each process" << endl << "# format: algorithm graph_type process time" << endl;
            size_t j = 0;

            for (std::vector<long>::iterator i = all_results.begin(); i != all_results.end(); ++i, ++j) {
                boost::posix_time::time_duration time = boost::posix_time::microseconds(*i);
                cout << "PageRank " << GET_TYPE_NAME(f.g) << " " << j << " " << time << endl;
            }

            cout << endl;
        }
    }

    // print the average
    {
        if (f.i_am_the_root_proc()) {
            cout << "# Average wall clock time" << endl << "# format: algorithm graph_type average_time" << endl;
            cout << "PageRank " << GET_TYPE_NAME(f.g) << " " << avg << endl << endl;
        }
    }

}

int BOOST_TEST_CALL_DECL main(int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
