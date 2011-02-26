#pragma once
#ifndef BENCHMARK_HPP_778YQPEJ
#define BENCHMARK_HPP_778YQPEJ

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/current_function.hpp>
#include <vector>
#include <string>
#define BENCHMARK(list,exp) \
    { \
        benchmark::timer t(&list,#exp,__FILE__,__LINE__,BOOST_CURRENT_FUNCTION); \
        _Pragma("pomp inst begin(#exp##_##__LINE__)") \
        (exp); \
        _Pragma("pomp inst end(#exp##_##__LINE__)") \
    }

namespace benchmark
{
    struct event {
        const bool operator<(const event &other) const;
        friend std::ostream& operator<<(std::ostream &out, event &ev);

        std::string exp, file, function;
        boost::posix_time::time_duration time;
        unsigned int line, count;
    };

    class event_list
    {
    public:
        friend std::ostream& operator<<(std::ostream &out, event_list &el);
        event& max();
        boost::posix_time::time_duration average_time();
        void push_back(const event &ev);
        size_t size() const;
        const event front() const;

    private:
        typedef std::vector<event> actual_list_type;
        actual_list_type actual_list;
    };

    class timer
    {
    public:
        timer(event_list *list, const std::string exp, const std::string file, const unsigned int line, const std::string function);
        virtual ~timer();

    private:
        event_list *_list;
        const std::string _exp, _file;
        const unsigned int _line;
        const std::string _function;
        boost::posix_time::ptime *start, *end;
        std::ostringstream oss;
    };
}

#endif /* end of include guard: BENCHMARK_HPP_778YQPEJ */
