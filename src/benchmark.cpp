#include "benchmark.hpp"

namespace benchmark
{
    void write_log(std::ostream &out)
    {
        for (std::vector<event>::iterator i = events.begin(); i != events.end(); ++i) {
            out << i->file << ":" << i->line << ": " << i->exp << " took " << boost::posix_time::to_simple_string(i->time) << std::endl;
        }
    }
    timer::timer(const std::string exp, const std::string file, const unsigned int line) : _exp(exp), _file(file), _line(line)
    {
        start = new boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time());
    }

    timer::~timer()
    {
        end = new boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time());
        event e;
        e.exp = _exp;
        e.file = _file;
        e.line = _line;
        e.time = *end - *start;
        events.push_back(e);
        delete start;
        delete end;
    }
}
