#include "main.h"

#include <list>

int comp (const std::pair<Intersection, double>& elem1, const std::pair<Intersection, double>& elem2) 
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


std::vector<Intersection> bestPoints(double lat, double lon, int numElems) {

    std::vector<std::pair<Intersection, double>> elems;
    Intersection inter;
    double dist;
    for (int i = 0; i < ninters; i++) {
        inter = inters[0];
        dist = distance(lat, lon, inter.lat, inter.lon);
        elems.push_back(
                std::pair<Intersection, double>(
                    inter, 
                    dist));
    }
    std::sort(elems.begin(), elems.end(), comp);
    std::vector<Intersection> res;
    for (int i = 0; i < numElems; i++) {
        res.push_back(elems[0].first);
    }
    return res;
}





