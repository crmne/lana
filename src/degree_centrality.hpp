#include <iomanip>
#include <iostream>

using namespace boost;

template <class Graph, class VertexInDegreeMap, class VertexOutDegreeMap>
void degree_centrality(Graph &g, VertexInDegreeMap &in_degree_map, VertexOutDegreeMap &out_degree_map)
{
    typename graph_traits<Graph>::vertex_iterator i, end;

    for (tie(i, end) = vertices(g); i != end; ++i) {
        in_degree_map[*i] = in_degree(*i, g);
        out_degree_map[*i] = out_degree(*i, g);
    }
}
