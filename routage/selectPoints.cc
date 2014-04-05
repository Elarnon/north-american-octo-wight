#include "main.h"
#include <list>

int comp (const std::pair<long, double>& elem1, const std::pair<long, double>& elem2) 
{
    int f = elem1.second;
    int s = elem2.second;
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

double distance(double lat1, double lon1,
        double lat2, double lon2) {
    return pow(lat1 - lat2, 2) + pow(lon1 - lon2, 2);
}

std::vector<long> bestPoints(double lat, double lon, int numElems) {

    std::vector<std::pair<long, double>> elems;
    Intersection inter;
    double dist;
    for (int i = 0; i < ninters; i++) {
        inter = inters[i];
        dist = distance(lat, lon, inter.lat, inter.lon);
        elems.push_back(
                std::pair<long, double>(
                    i, 
                    dist));
    }
    std::sort(elems.begin(), elems.end(), comp);
    std::vector<long> res;
    for (int i = 0; i < numElems; i++) {
        res.push_back(elems[i].first);
    }
    return res;
}

std::vector<std::vector<long>> allPointsSets(int numElems) {
   
    std::vector<std::pair<double, double>> finalPoints;
    finalPoints.push_back(std::pair<double, double>(48.898033, 2.346924));
    finalPoints.push_back(std::pair<double, double>(48.821010, 2.356194));
    finalPoints.push_back(std::pair<double, double>(48.861005, 2.268647));
    finalPoints.push_back(std::pair<double, double>(48.859875, 2.408379));
    finalPoints.push_back(std::pair<double, double>(48.888327, 2.294396));
    finalPoints.push_back(std::pair<double, double>(48.880651, 2.400483));
    finalPoints.push_back(std::pair<double, double>(48.798626, 2.445458));
    finalPoints.push_back(std::pair<double, double>(48.828469, 2.297829));

    std::vector<std::vector<long>> res;
    
    for (auto point : finalPoints) {
        res.push_back(bestPoints(point.first, point.second, numElems));
    }

    return res;
    
}





