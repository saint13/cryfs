#pragma once
#ifndef MESSMER_CRYFS_SRC_CONFIG_CRYPTO_KEYCONFIG_H
#define MESSMER_CRYFS_SRC_CONFIG_CRYPTO_KEYCONFIG_H

#include <messmer/cpp-utils/data/Data.h>
#include <iostream>

namespace cryfs {

    class DerivedKeyConfig {
    public:
        DerivedKeyConfig(cpputils::Data salt, uint64_t N, uint32_t r, uint32_t p)
                : _salt(std::move(salt)),
                  _N(N), _r(r), _p(p) { }

        DerivedKeyConfig(DerivedKeyConfig &&rhs) = default;

        const cpputils::Data &salt() const {
            return _salt;
        }

        size_t N() const {
            return _N;
        }

        size_t r() const {
            return _r;
        }

        size_t p() const {
            return _p;
        }

        void save(std::ostream &stream) const;

        static DerivedKeyConfig load(std::istream &stream);

    private:
        cpputils::Data _salt;
        uint64_t _N;
        uint32_t _r;
        uint32_t _p;

        DISALLOW_COPY_AND_ASSIGN(DerivedKeyConfig);
    };

}

#endif