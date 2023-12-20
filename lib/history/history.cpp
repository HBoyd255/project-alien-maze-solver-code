
#include "history.h"

#include <algorithm>

History::History(uint16_t maxValues) : _maxValues(maxValues) {}

History::operator String() const {
    String stringToReturn = "(";

    for (const auto& element : this->_values) {
        stringToReturn += element;
        stringToReturn += ",";
    }

    if (stringToReturn.length() > 1) {
        stringToReturn.remove(stringToReturn.length() - 1);
    }

    stringToReturn += ")";

    return stringToReturn;
}

void History::add(int32_t newValue) {
    if (this->_values.size() == this->_maxValues) {
        this->_values.erase(this->_values.begin());
    }
    this->_values.push_back(newValue);
}

int16_t History::getMedian() {
    uint16_t medianIndex = this->_values.size() / 2;

    std::vector<int16_t> valuesCopy = this->_values;

    std::sort(valuesCopy.begin(), valuesCopy.end());

    return valuesCopy[medianIndex];
}
