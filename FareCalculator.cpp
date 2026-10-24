#include "FareCalculator.h"

namespace {
constexpr double kBaseFare = 10.0;
constexpr double kFarePerKilometre = 5.0;
}

double FareCalculator::calculateFare(const RouteResult& route) {
    if (!route.found) {
        return 0.0;
    }
    return kBaseFare + route.totalDistanceKm * kFarePerKilometre;
}