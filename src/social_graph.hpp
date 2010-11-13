#include <boost/mpi/communicator.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>

using namespace boost;

struct Person {
    Person() {}
    Person(const std::string& name) : name(name) {}

    std::string name;
    unsigned int in_degree, out_degree;

    // Serialization support is required!
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & name & in_degree & out_degree;
    }
};

// Enable named vertex support
namespace boost
{
    namespace graph
    {
        template<>
        struct internal_vertex_name<Person> {
            typedef multi_index::member<Person, std::string, &Person::name> type;
        };

        template<>
        struct internal_vertex_constructor<Person> {
            typedef vertex_from_name<Person> type;
        };
    }
}

// Declare the graph type and vertex and edge types
typedef adjacency_list <vecS, distributedS<boost::graph::distributed::mpi_process_group, vecS>, bidirectionalS, Person> Graph;
typedef property_map<Graph, std::string Person::*>::type VertexNameMap;
typedef property_map<Graph, unsigned int Person::*>::type VertexInDegreeMap;
typedef property_map<Graph, unsigned int Person::*>::type VertexOutDegreeMap;

// custom label writer
template <class Name, class InDegree, class OutDegree>
class nd_label_writer
{
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
make_nd_label_writer(Name n, InDegree i, OutDegree o)
{
    return nd_label_writer<Name, InDegree, OutDegree>(n, i, o);
}