

#include "comparison.h"

bool within(int valueA, int valueB, int range) {
    return abs(valueA - valueB) <= range;
}