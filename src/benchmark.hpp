#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <ostream>
#define BENCHMARK(exp) { benchmark::timer t(#exp,__FILE__,__LINE__); (exp); }

namespace benchmark
{
    struct event {
        std::string exp, file;
        boost::posix_time::time_duration time;
        unsigned int line;
    };

    std::vector<event> events;

    inline void write_log(std::ostream &out)
    {
        for (std::vector<event>::iterator i = events.begin(); i != events.end(); ++i) {
            out << i->file << ":" << i->line << ": " << i->exp << " took " << boost::posix_time::to_simple_string(i->time) << std::endl;
        }
    }

    class timer
    {
    public:
        timer(const std::string exp, const std::string file, const unsigned int line) : _exp(exp), _file(file), _line(line) {
            start = new boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time());
        }

        virtual ~timer() {
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

    private:
        const std::string _exp, _file;
        const unsigned int _line;
        boost::posix_time::ptime *start, *end;
        std::ostringstream oss;
    };
}


