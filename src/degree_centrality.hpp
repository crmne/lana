// This is a sligthly modified version of boost/graph/degree_centrality.hpp, to make it work with distributed graphs
#pragma once
#ifndef DEGREE_CENTRALITY_HPP_TBVK5D16
#define DEGREE_CENTRALITY_HPP_TBVK5D16

#include <boost/graph/distributed/concepts.hpp>

using namespace boost;

template <typename Graph>
struct degree_centrality_measure {
    typedef typename graph_traits<Graph>::degree_size_type degree_type;
    typedef typename graph_traits<Graph>::vertex_descriptor vertex_type;
};

template <typename Graph>
struct popularity_measure : public degree_centrality_measure<Graph> {
    typedef degree_centrality_measure<Graph> base_type;
    typedef typename base_type::degree_type degree_type;
    typedef typename base_type::vertex_type vertex_type;

    inline degree_type operator()(vertex_type v, const Graph& g) {
        function_requires< BidirectionalGraphConcept<Graph> >();
        return in_degree(v, g);
    }
};

template <typename Graph>
inline popularity_measure<Graph>
measure_popularity(const Graph&)
{
    return popularity_measure<Graph>();
}

template <typename Graph>
struct gregariousness_measure : public degree_centrality_measure<Graph> {
    typedef degree_centrality_measure<Graph> base_type;
    typedef typename base_type::degree_type degree_type;
    typedef typename base_type::vertex_type vertex_type;

    inline degree_type operator()(vertex_type v, const Graph& g) {
        function_requires< IncidenceGraphConcept<Graph> >();
        return out_degree(v, g);
    }
};

template <typename Graph>
inline gregariousness_measure<Graph>
measure_gregariousness(const Graph&)
{
    return gregariousness_measure<Graph>();
}

template <typename Graph, typename Vertex, typename Measure>
inline typename Measure::degree_type
degree_centrality(const Graph& g, Vertex v, Measure measure)
{
    function_requires< DegreeMeasureConcept<Measure, Graph> >();
    return measure(v, g);
}

template <typename Graph, typename Vertex>
inline typename graph_traits<Graph>::degree_size_type
degree_centrality(const Graph& g, Vertex v)
{
    return degree_centrality(g, v, measure_gregariousness(g));
}


// These are alias functions, intended to provide a more expressive interface.

template <typename Graph, typename Vertex>
inline typename graph_traits<Graph>::degree_size_type
gregariousness(const Graph& g, Vertex v)
{
    return degree_centrality(g, v, measure_gregariousness(g));
}

template <typename Graph, typename Vertex>
inline typename graph_traits<Graph>::degree_size_type
popularity(const Graph& g, Vertex v)
{
    return degree_centrality(g, v, measure_popularity(g));
}


template <typename Graph, typename CentralityMap, typename Measure>
inline void
all_degree_centralities(const Graph& g, CentralityMap cent, Measure measure)
{
    // function_requires< VertexListGraphConcept<Graph> >(); TODO: find a suitable concept
    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename graph_traits<Graph>::vertex_iterator VertexIterator;
    function_requires< WritablePropertyMapConcept<CentralityMap, Vertex> >();
    typedef typename property_traits<CentralityMap>::value_type Centrality;

    VertexIterator i, end;

    for (tie(i, end) = vertices(g); i != end; ++i) {
        Centrality c = degree_centrality(g, *i, measure);
        put(cent, *i, c);
    }
}

template <typename Graph, typename CentralityMap>
inline void all_degree_centralities(const Graph& g, CentralityMap cent)
{
    all_degree_centralities(g, cent, measure_gregariousness(g));
}

// More helper functions for computing gregariousness and popularity.
// I hate the names of these functions, but gregariousness and popularity
// don't pluralize too well.

template <typename Graph, typename CentralityMap>
inline void all_gregariousness_values(const Graph& g, CentralityMap cent)
{
    all_degree_centralities(g, cent, measure_gregariousness(g));
}

template <typename Graph, typename CentralityMap>
inline void all_popularity_values(const Graph& g, CentralityMap cent)
{
    all_degree_centralities(g, cent, measure_popularity(g));
}

#endif /* end of include guard: DEGREE_CENTRALITY_HPP_TBVK5D16 */
