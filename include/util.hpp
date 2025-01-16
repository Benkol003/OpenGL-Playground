#include <stdexcept>
#include <vector>

namespace util {


    template<typename value_t> std::vector<value_t> range_n(value_t start, value_t end, size_t n) {
        //generate range of values from start to end inclusive of n values
        value_t step = ((end - start) / (n - 1));
        std::vector<value_t> ret; ret.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            ret.push_back(start);
            start += step;
        }
        return ret;
    }



	class NotImplementedException : public std::logic_error
	{
	public:
		NotImplementedException() : std::logic_error("Not Implemented") {};
	};
}