//
// Created by Parti on 2021/1/31.
//

#pragma once

#include <json/json.h>
#include <string>
#include <trantor/utils/Date.h>

namespace tech::utils {
    namespace misc {
        uint64_t uniform_random(uint64_t size = 99999999);

        std::string fromDate(const uint64_t &milliseconds = 0);

        std::string fromDate(const trantor::Date &date);

        trantor::Date toDate(const std::string &date);

        trantor::Date toDate();
    }
}
