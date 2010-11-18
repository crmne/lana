#include <boost/program_options.hpp>

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/graphviz.hpp>

#include <iostream>
#include <iterator>
#include <vector>

#include "social_graph.hpp"
#include "degree_centrality.hpp"
#include "node_strength.hpp"
#include "graph_loaders.hpp"

namespace po = boost::program_options;

void usage(char *execname, po::options_description &desc)
{
    std::cout << "Usage: " << execname << " [options] <filename>" << std::endl << desc;
}

int main(int argc, char *argv[])
{
    bool verbose;
    std::string ofile, ifile, iformat, oformat;
    std::istream *input;
    std::ostream *output;

    mpi::environment env(argc, argv);
    mpi::communicator world;

    Graph g;
    VertexNameMap name_map = get(&Person::name, g);
    VertexUIntMap popularity_map = get(&Person::in_degree, g);
    VertexUIntMap gregariousness_map = get(&Person::out_degree, g);
    VertexUIntMap prestige_map = get(&Person::in_strength, g);
    VertexUIntMap activity_map = get(&Person::out_strength, g);
    EdgeUIntMap edge_weight_map = get(&Relationship::weight, g);

    bool is_root_proc = (process_id(g.process_group()) == 0);

    // Option definition
    po::options_description generic_opts("Generic options");
    generic_opts.add_options()
    ("help,h", "print this message")
    ("verbose,v", po::bool_switch(&verbose), "print some info while processing")
    ;

    po::options_description io_opts("Input/Output options");
    io_opts.add_options()
    ("output,o", po::value(&ofile), "save output to file 'arg'")
    ("input-format,f", po::value(&iformat), "treat input as [csv|dot|metis|gml]")
    ;

    po::options_description hidden_opts("Hidden options");
    hidden_opts.add_options()
    ("input-file", po::value(&ifile), "input file");

    // first command line token with no name: input-file
    po::positional_options_description p;
    p.add("input-file", -1);

    po::options_description cmdline_opts;
    cmdline_opts.add(generic_opts).add(io_opts).add(hidden_opts);

    po::options_description visible_opts;
    visible_opts.add(generic_opts).add(io_opts);
    // end of option definition

    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(cmdline_opts).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            if (is_root_proc) {
                usage(argv[0], visible_opts);
            }

            return 0;
        }

        if (vm.count("output")) {
            output = new std::ofstream(ofile.c_str());

        } else {
            output = &std::cout;
        }

        if (is_root_proc) {
            if (vm.count("input-file")) {
                input = new std::ifstream(ifile.c_str());

            } else {
                input = &std::cin;
            }

            sigsna::load_graph_from_csv(*input, g);
        }

    } catch (std::exception& e) {
        if (is_root_proc) {
            std::cout << "Error: " << e.what() << std::endl;
            usage(argv[0], visible_opts);
        }

        return 1;
    }

    synchronize(g.process_group());

    all_popularity_values(g, popularity_map);
    all_gregariousness_values(g, gregariousness_map);
    all_prestige_values(g, prestige_map, edge_weight_map);
    all_activity_values(g, activity_map, edge_weight_map);

    write_graphviz(*output, g, make_all_measures_writer(name_map, popularity_map, gregariousness_map, prestige_map, activity_map));

    return 0;
}