#include <algorithm>
#include "benchmark.hpp"

namespace benchmark
{
    const bool event::operator<(const event &other) const
    {
        return this->time < other.time;
    }

    std::ostream& operator<<(std::ostream &out, event &ev)
    {
        return out << ev.count << ":" << ev.file << ":" << ev.line + "@" << ev.function << ":" << ev.exp << ": " << ev.time;
    }

    std::ostream& operator<<(std::ostream &out, event_list &el)
    {
        for (event_list::actual_list_type::iterator i = el.actual_list.begin(); i != el.actual_list.end(); ++i) {
            out << *i << std::endl;
        }

        return out;
    }

    void event_list::push_back(const event &ev)
    {
        actual_list.push_back(ev);
    }

    size_t event_list::size() const
    {
        return actual_list.size();
    }

    event& event_list::max()
    {
        return *std::max_element(actual_list.begin(), actual_list.end());
    }

    boost::posix_time::time_duration event_list::average_time()
    {
        boost::posix_time::time_duration sum(0, 0, 0, 0);

        for (actual_list_type::iterator i = actual_list.begin(); i != actual_list.end(); ++i) {
            sum += i->time;
        }

        return sum / actual_list.size();
    }

    timer::timer(event_list *list, const std::string exp, const std::string file, const unsigned int line, const std::string function) : _list(list), _exp(exp), _file(file), _line(line), _function(function)
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
        e.function = _function;
        e.time = *end - *start;
        e.count = _list->size() + 1;
        _list->push_back(e);
        delete start;
        delete end;
    }
}
