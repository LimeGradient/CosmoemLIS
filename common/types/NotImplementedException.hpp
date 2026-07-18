#pragma once

#include <stdexcept>

namespace Cosmoem {
    class NotImplementedException : public std::logic_error {
    public:
        NotImplementedException() : std::logic_error("Function not yet implemented") { }
    };
}