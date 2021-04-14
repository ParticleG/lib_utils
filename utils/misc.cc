//
// Created by Parti on 2021/1/31.
//

#include <fstream>
#include <random>
#include <utils/misc.h>
#include <thread>
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

std::string misc::getFileString(std::string_view path) {
    constexpr auto read_size = std::size_t{4096};
    auto stream = ifstream{path.data()};
    stream.exceptions(ios_base::badbit);

    auto out = std::string{};
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

void misc::logger(const string &className, const string &message) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    LOG_DEBUG << "(" << ss.str() << ")[" << className << "] " << message;
}