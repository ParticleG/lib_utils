//
// Created by Parti on 2021/1/31.
//

#include <random>
#include <utils/Utils.h>


using namespace std;
using namespace tech::utils;
using namespace trantor;

unsigned long Utils::uniform_random(unsigned int size) {
    mt19937 generator(Date::now().microSecondsSinceEpoch());
    uniform_int_distribution<int> dis(0, size);
    return dis(generator);
}

string Utils::fromDate(const uint64_t &milliseconds) {
    return Date::now().after(milliseconds).toDbStringLocal();
}

std::string Utils::fromDate(const Date &date) {
    return date.toDbStringLocal();
}

trantor::Date Utils::toDate(const string &date) {
    return Date::fromDbStringLocal(date);
}

trantor::Date Utils::toDate() {
    return Date::now();
}
