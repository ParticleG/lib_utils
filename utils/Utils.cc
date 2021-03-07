//
// Created by Parti on 2021/1/31.
//

#include <random>
#include <utils/Utils.h>


using namespace std;
using namespace tech::utils;
using namespace trantor;

uint64_t Utils::uniform_random(uint64_t size) {
    mt19937 generator(static_cast<unsigned int>(Date::now().microSecondsSinceEpoch()));
    uniform_int_distribution<uint64_t> dis(0, size);
    return dis(generator);
}

string Utils::fromDate(const uint64_t &milliseconds) {
    return Date::now().after(static_cast<double>(milliseconds)).toDbStringLocal();
}

string Utils::fromDate(const Date &date) {
    return date.toDbStringLocal();
}

trantor::Date Utils::toDate(const string &date) {
    return Date::fromDbStringLocal(date);
}

trantor::Date Utils::toDate() {
    return Date::now();
}
