#include <boost/progress.hpp>
#include <vector>
#include <ostream>
#define BENCHMARK(exp) { benchmark_timer(#exp,__FILE__,__LINE__); (exp); }

struct timed_event {
    std::string exp, file, time;
    unsigned int line;
};

namespace benchmark
{
    std::vector<timed_event> events;
    inline void write_log(std::ostream &out)
    {
        for (std::vector<timed_event>::iterator i = events.begin(); i != events.end(); ++i) {
            out << i->file << ":" << i->line << ": " << i->exp << " took " << i->time;
        }
    }
}

class benchmark_timer
{
public:
    benchmark_timer(const std::string exp, const std::string file, const unsigned int line) : _exp(exp), _file(file), _line(line) {
        t = new progress_timer(oss);
    }

    virtual ~benchmark_timer() {
        delete t;
        timed_event event;
        event.exp = _exp;
        event.file = _file;
        event.line = _line;
        event.time = oss.str();
        benchmark::events.push_back(event);
    }

private:
    const std::string _exp, _file;
    const unsigned int _line;
    progress_timer *t;
    std::ostringstream oss;
};
