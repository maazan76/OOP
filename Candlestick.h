#ifndef CANDLESTICK_H
#define CANDLESTICK_H

#include <string>
#include <iostream>

class Candlestick {
private:
    std::string date;
    double open;
    double close;
    double high;
    double low;

public:
    Candlestick(const std::string& date,
                double open,
                double close,
                double high,
                double low);

    void display() const;
};


#endif // CANDLESTICK_H
