#include <string>
#include <vector>
#include <map>

namespace sigsna {
template<class Graph>
void load_graph_from_csv(CSV &csv, Graph &g)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    Vertex follower, followed;
    std::vector<std::string> row;
    while(!(row = csv.nextRow()).empty())
    {
        add_edge(row[0], row[1], g);
    }
}
} // end of namespace sigsna
