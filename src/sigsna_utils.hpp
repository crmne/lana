#include <string>
#include <vector>
#include <map>

namespace sigsna {
template<typename Graph, typename VertexPropertyMap>
void load_graph_from_csv(CSV &csv, Graph &g, VertexPropertyMap &name_map)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

    Vertex follower, followed;
    std::vector<std::string> row;
    std::map<std::string, unsigned int> id_map;
    std::pair<std::map<std::string, unsigned int>::iterator, bool> ins_ret;
    while(!(row = csv.nextRow()).empty())
    {
        ins_ret = id_map.insert(std::pair<std::string, unsigned int>(row[0], id_map.size()));
        if(ins_ret.second) {
            follower = add_vertex(g);
            name_map[follower] = row[0];
        } else {
            follower = vertex(ins_ret.first->second, g);
        }

        ins_ret = id_map.insert(std::pair<std::string, unsigned int>(row[1], id_map.size()));
        if(ins_ret.second) {
            followed = add_vertex(g);
            name_map[followed] = row[1];
        } else {
            followed = vertex(ins_ret.first->second, g);
        }

        add_edge(follower, followed, g);
    }
}
} // end of namespace sigsna
