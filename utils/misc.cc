//
// Created by Parti on 2021/1/31.
//

#include <random>
#include <utils/misc.h>
#include <trantor/utils/Logger.h>


using namespace std;
using namespace tech::utils;
using namespace trantor;

uint64_t misc::uniform_random(uint64_t size) {
    mt19937 generator(static_cast<unsigned int>(Date::now().microSecondsSinceEpoch()));
    uniform_int_distribution<uint64_t> dis(0, size);
    return dis(generator);
}

string misc::fromDate(const uint64_t &milliseconds) {
    return fromDate(Date::now().after(static_cast<double>(milliseconds)));
}

string misc::fromDate(const Date &date) {
    return date.toDbStringLocal();
}

trantor::Date misc::toDate(const string &date) {
    return Date::fromDbStringLocal(date);
}

trantor::Date misc::toDate() {
    return Date::now();
}
