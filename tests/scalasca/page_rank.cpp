#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/page_rank.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

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

int main(int argc, char *argv[])
{
#pragma pomp inst init
    mpi::environment env(argc, argv);
    SmallWorld::Fixture<SmallWorldDigraph> f(653646, 86, 0.05);
#pragma pomp inst begin(pagerank)
    page_rank(f.graph(), f.pagerank_map());
#pragma pomp inst end(pagerank)
}
