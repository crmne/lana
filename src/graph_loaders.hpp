#include <vector>
#include "csv.hpp"

namespace sigsna
{
    template<class Graph>
    void load_graph_from_csv(CSV &csv, Graph &g)
    {
        typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

        Vertex follower, followed;
        std::vector<std::string> row;

        while (!(row = csv.nextRow()).empty()) {
            add_edge(row[0], row[1], g);
        }
    }
    template<class Graph>
    void load_graph_from_csv(std::istream &file, Graph &g)
    {
        CSV csv(file);
        load_graph_from_csv(csv, g);
    }
}
