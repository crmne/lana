#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <ostream>
#define BENCHMARK(exp) { benchmark_timer t(#exp,__FILE__,__LINE__); (exp); }

struct timed_event {
    std::string exp, file;
    boost::posix_time::time_duration time;
    unsigned int line;
};

namespace benchmark
{
    std::vector<timed_event> events;
    inline void write_log(std::ostream &out)
    {
        for (std::vector<timed_event>::iterator i = events.begin(); i != events.end(); ++i) {
            out << i->file << ":" << i->line << ": " << i->exp << " took " << boost::posix_time::to_simple_string(i->time) << std::endl;
        }
    }
}

class benchmark_timer
{
public:
    benchmark_timer(const std::string exp, const std::string file, const unsigned int line) : _exp(exp), _file(file), _line(line) {
        start = new boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time());
    }

    virtual ~benchmark_timer() {
        end = new boost::posix_time::ptime(boost::posix_time::microsec_clock::universal_time());
        timed_event event;
        event.exp = _exp;
        event.file = _file;
        event.line = _line;
        event.time = *end - *start;
        benchmark::events.push_back(event);
        delete start;
        delete end;
    }

private:
    const std::string _exp, _file;
    const unsigned int _line;
    boost::posix_time::ptime *start, *end;
    std::ostringstream oss;
};
