#include <boost/mpi/communicator.hpp>
#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/graphviz.hpp>
#include <iomanip>
#include <iostream>
#include "csv.hpp"
#include "sigsna_utils.hpp"

using namespace boost;
using boost::graph::distributed::mpi_process_group;

struct Person {
    Person() {}
    Person(const std::string& name) : name(name) {}

    std::string name;
    unsigned int in_degree, out_degree;

    // Serialization support is required!
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int)
    {
        ar & name & in_degree & out_degree;
    }
};

// Enable named vertex support
namespace boost {
namespace graph {
template<>
struct internal_vertex_name<Person>
{
    typedef multi_index::member<Person, std::string, &Person::name> type;
};
template<>
struct internal_vertex_constructor<Person>
{
    typedef vertex_from_name<Person> type;
};
} // end of namespace graph
} // end of namespace boost

// Declare the graph type and vertex and edge types
typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>,
        bidirectionalS, Person> Graph;
typedef property_map<Graph, std::string Person::*>::type VertexNameMap;
typedef property_map<Graph, unsigned int Person::*>::type VertexInDegreeMap;
typedef property_map<Graph, unsigned int Person::*>::type VertexOutDegreeMap;

// custom label writer
template <class Name, class InDegree, class OutDegree>
class nd_label_writer {
public:
    nd_label_writer(Name _name, InDegree _in_degree, OutDegree _out_degree) : name(_name), in_degree(_in_degree), out_degree(_out_degree) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        // "in_degree" and "out_degree" are not a DOT attributes, so they will be ignored by the plotting programs
        // see http://www.graphviz.org/doc/schema/attributes.xml
        out << "[label=\"" << name[v] << "\" in_degree=" << in_degree[v] << " out_degree=" << out_degree[v] << "]";
    }
private:
    Name name;
    InDegree in_degree;
    OutDegree out_degree;
};
template <class Name, class InDegree, class OutDegree>
inline nd_label_writer<Name, InDegree, OutDegree>
make_nd_label_writer(Name n, InDegree i, OutDegree o) {
    return nd_label_writer<Name, InDegree, OutDegree>(n, i, o);
}

template <class Graph, class VertexInDegreeMap, class VertexOutDegreeMap>
void degree_centrality(Graph &g, VertexInDegreeMap &in_degree_map, VertexOutDegreeMap &out_degree_map)
{
    typename graph_traits<Graph>::vertex_iterator i, end;
    for(tie(i, end) = vertices(g); i != end; ++i) {
        in_degree_map[*i] = in_degree(*i, g);
        out_degree_map[*i] = out_degree(*i, g);
    }
}

int main (int argc, char *argv[])
{
    mpi::environment env(argc, argv);
    mpi::communicator world;
    std::cout << "Process " << world.rank() << " of " << world.size() << " reporting for duty." << std::endl;

    if(argc < 2)
    {
        std::cerr << "What CSV file do you want me to load?" << std::endl;
        return EXIT_FAILURE;
    }

    Graph g;
    VertexNameMap name_map = get(&Person::name, g);
    VertexInDegreeMap in_degree_map = get(&Person::in_degree, g);
    VertexOutDegreeMap out_degree_map = get(&Person::out_degree, g);

    // Load the graph from SIGSNA's CSV
    if (process_id(g.process_group()) == 0) {
        std::cout << "Process 0 here, sir! Loading the graph... ";
        std::cout.flush();

        std::ifstream file(argv[1]);
        CSV csv(file);
        sigsna::load_graph_from_csv(csv, g);

        std::cout << "done" << std::endl;
    }
    synchronize(g.process_group());

    degree_centrality(g, in_degree_map, out_degree_map);

    write_graphviz(std::cout, g, make_nd_label_writer(name_map, in_degree_map, out_degree_map));

    return EXIT_SUCCESS;
}
