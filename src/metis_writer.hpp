#include <boost/property_map/parallel/global_index_map.hpp>
using namespace boost;

template<typename Graph>
void write_metis(std::ostream &out, const Graph &g)
{
    const size_t total_vertices = num_vertices(g);
    const size_t total_edges = num_edges(g);

    typedef typename property_map<Graph, vertex_index_t>::const_type  VertexIndexMap;
    typedef typename property_map<Graph, vertex_global_t>::const_type  VertexGlobalMap;

    typedef typename boost::graph::parallel::process_group_type<Graph>::type process_group_type;
    process_group_type pg = process_group(g);

    parallel::global_index_map<VertexIndexMap, VertexGlobalMap> global_index(pg, num_vertices(g), get(vertex_index, g), get(vertex_global, g));

    out << total_vertices << " " << total_edges << std::endl;

    typename graph_traits<Graph>::vertex_iterator v, vend;

    for (tie(v, vend) = vertices(g); v != vend; ++v) {
        out << get(global_index, *v);

        typename graph_traits<Graph>::adjacency_iterator a, aend;

        for (tie(a, aend) = adjacent_vertices(*v, g); a != aend; ++a) {
            out << " " << get(global_index, *a);
        }

        out << std::endl;
    }
}
