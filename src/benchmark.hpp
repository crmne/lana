#pragma once
#ifndef BENCHMARK_HPP_778YQPEJ
#define BENCHMARK_HPP_778YQPEJ

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

    static std::vector<event> events;

    void write_log(std::ostream &out);

    class timer
    {
    public:
        timer(const std::string exp, const std::string file, const unsigned int line);
        virtual ~timer();

    private:
        const std::string _exp, _file;
        const unsigned int _line;
        boost::posix_time::ptime *start, *end;
        std::ostringstream oss;
    };
}

#endif /* end of include guard: BENCHMARK_HPP_778YQPEJ */
