#include "Candlestick.h"
#include <iomanip>

Candlestick::Candlestick(const std::string& _date, double _open, double _close, double _high, double _low)
: date(_date), open(_open), close(_close), high(_high), low(_low) {}

void Candlestick::display() const {
    std::cout << std::left << std::setw(15) << date << " | "
              << std::setw(10) << open << " | "
              << std::setw(10) << high << " | "
              << std::setw(10) << low  << " | "
              << std::setw(10) << close << std::endl;
}