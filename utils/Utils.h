//
// Created by Parti on 2021/1/31.
//

#pragma once

#include <json/json.h>
#include <string>
#include <trantor/utils/Date.h>

namespace tech::utils {
    class Utils {
    public:
        static unsigned long uniform_random(unsigned int size = 99999999);

        static std::string fromDate(const uint64_t &milliseconds = 0);

        static std::string fromDate(const trantor::Date &date);

        static trantor::Date toDate(const std::string &date);

        static trantor::Date toDate();
    };
}
