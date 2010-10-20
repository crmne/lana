#include <boost/graph/graphviz.hpp>
#include <boost/graph/degree_centrality.hpp>
#include <iomanip>
#include <iostream>
#include "csv.hpp"
#include "sigsna_utils.hpp"

using namespace boost;

// Declare the graph type and vertex and edge types
typedef property<vertex_name_t, std::string,
        property<vertex_degree_t, unsigned int> > VertexProperties;
typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperties> Graph;
typedef property_map<Graph, vertex_name_t>::type VertexNameMap;
typedef property_map<Graph, vertex_degree_t>::type VertexDegreeMap;

// custom label writer
template <class Name, class Degree>
class nd_label_writer {
public:
    nd_label_writer(Name _name, Degree _degree) : name(_name), degree(_degree) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        // "degree" is not a DOT attribute, so it will be ignored by the plotting programs
        // see http://www.graphviz.org/doc/schema/attributes.xml
        out << "[label=\"" << name[v] << "\" degree=" << degree[v] << "]";
    }
private:
    Name name;
    Degree degree;
};
template <class Name, class Degree>
inline nd_label_writer<Name, Degree>
make_nd_label_writer(Name n, Degree d) {
    return nd_label_writer<Name, Degree>(n, d);
}

int main (int argc, char const *argv[])
{
    if(argc < 2)
    {
        std::cerr << "What CSV file do you want me to load?" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream file(argv[1]);
    CSV csv(file);

    Graph g;
    VertexNameMap name_map = get(vertex_name, g);
    VertexDegreeMap degree_map = get(vertex_degree, g);

    // Load the graph from SIGSNA's CSV
    sigsna::load_graph_from_csv(csv, g, name_map);

    // Compute the degree centrality for graph.
    all_degree_centralities(g, degree_map);

    write_graphviz(std::cout, g, make_nd_label_writer(name_map, degree_map));

    return EXIT_SUCCESS;
}
