#ifndef FARE_CALCULATOR_H
#define FARE_CALCULATOR_H

#include "RouteOptimizer.h"

class FareCalculator {
public:
    // Sample academic pricing rule: base fare plus a per-kilometre charge.
    static double calculateFare(const RouteResult& route);
};

#endif