#ifndef HISTORY_H
#define HISTORY_H

#include <Arduino.h>

#include <vector>

class History {
   public:
    History(uint16_t maxValues);

    operator String() const;

    void add(int32_t newValue);
    int16_t getMedian();

   private:
    std::vector<int16_t> _values;
    uint16_t _maxValues;
};

#endif  // HISTORY_H
