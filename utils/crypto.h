//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <string>

namespace tech::utils {
    namespace crypto {
        std::string blake2b(const std::string &source, const unsigned int &divider = 2);
        std::string keccak(const std::string &source, const unsigned int &divider = 2);
    }
}

