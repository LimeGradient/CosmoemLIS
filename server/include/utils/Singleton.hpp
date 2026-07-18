#pragma once

namespace Cosmoem::Utils {
    template <class T>
    class Singleton {
    public:
        static T* get() {
            static T instance;
            return &instance;
        }
    };
}