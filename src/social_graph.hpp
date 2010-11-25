#pragma once
#ifndef SOCIAL_GRAPH_HPP_M6Y1UN4Z
#define SOCIAL_GRAPH_HPP_M6Y1UN4Z

#include <boost/mpi/communicator.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>

using namespace boost;

struct Person {
    Person() {}
    Person(const std::string& name) : name(name) {}

    std::string name;
    unsigned int in_degree, out_degree, in_strength, out_strength;
    float pagerank;

    // Serialization support is required!
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & name & in_degree & out_degree & in_strength & out_strength & pagerank;
    }
};

struct Relationship {
    Relationship(const unsigned int _weight = 0) {
        weight = _weight;
    }

    unsigned int weight;
    // Serialization support is required!
    template<typename Archiver>
    void serialize(Archiver &ar, const unsigned int) {
        ar & weight;
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
typedef adjacency_list <vecS, distributedS<boost::graph::distributed::mpi_process_group, vecS>, bidirectionalS, Person, Relationship> Graph;
typedef property_map<Graph, std::string Person::*>::type VertexNameMap;
typedef property_map<Graph, unsigned int Person::*>::type VertexUIntMap;
typedef property_map<Graph, float Person::*>::type VertexFloatMap;
typedef property_map<Graph, unsigned int Relationship::*>::type EdgeUIntMap;

// custom label writer
template <class Name, class InDegree, class OutDegree, class InStrength, class OutStrength, class PageRank>
class all_measures_writer
{
public:
    all_measures_writer(Name _name, InDegree _in_degree, OutDegree _out_degree, InStrength _in_strength, OutStrength _out_strength, PageRank _pagerank) : name(_name), in_degree(_in_degree), out_degree(_out_degree), in_strength(_in_strength), out_strength(_out_strength), pagerank(_pagerank) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        // things that are not DOT attributes will be ignored by the plotting programs
        // see http://www.graphviz.org/doc/schema/attributes.xml
        out << "[label=\"" << name[v] << "\" in_degree=" << in_degree[v] << " out_degree=" << out_degree[v] << " in_strength=" << in_strength[v] << " out_strength=" << out_strength[v] << " pagerank=" << pagerank[v] << "]";
    }
private:
    Name name;
    InDegree in_degree;
    OutDegree out_degree;
    InStrength in_strength;
    OutStrength out_strength;
    PageRank pagerank;
};

template <class Name, class InDegree, class OutDegree, class InStrength, class OutStrength, class PageRank>
inline all_measures_writer<Name, InDegree, OutDegree, InStrength, OutStrength, PageRank>
make_all_measures_writer(Name n, InDegree i, OutDegree o, InStrength s, OutStrength ss, PageRank pr)
{
    return all_measures_writer<Name, InDegree, OutDegree, InStrength, OutStrength, PageRank>(n, i, o, s, ss, pr);
}

#endif /* end of include guard: SOCIAL_GRAPH_HPP_M6Y1UN4Z */
