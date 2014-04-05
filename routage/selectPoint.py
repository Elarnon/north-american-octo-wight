from parse import *
from diaspora import main_diaspora

GAIN_MULT = 5

def distance(x1, y1, x2, y2):
    return (x1-x2)**2 + (y2-y1)**2

def trivial(cars, inters, rues, time, nvehic, start, all_cars):
    
    threshold = 1e-6

    points = []
    pointsSets = []
    points.append((48.898033, 2.346924)) ## haut
    pointsSets.append([])
    points.append((48.821010, 2.356194)) ## bas
    pointsSets.append([])
    points.append((48.861005, 2.268647)) ## gauche
    pointsSets.append([])
    points.append((48.859875, 2.408379)) ## droite
    pointsSets.append([])
    points.append((48.888327, 2.294396)) ## haut gauche
    pointsSets.append([])
    points.append((48.880651, 2.400483)) ## haut droit
    pointsSets.append([])
    points.append((48.798626, 2.445458)) ## bas droit
    pointsSets.append([])
    points.append((48.828469, 2.297829)) ## bas gauche
    pointsSets.append([])

    maxDist = 0
    minDist = float('inf')

    for inter in inters:
        (x,y) = points[0]
        dist = distance(x, y, float(inter.lat), float(inter.lon))
        pointsSets[0].append((inter,dist))
        if dist < minDist:
            minDist = dist
        if dist > maxDist:
            maxDist = dist

    print(minDist)
    print(maxDist)
    print(len(pointsSets[0]))

cars, inters, rues, time, nvehic, start, all_cars = parse(sys.stdin)
trivial(cars, inters, rues, time, nvehic, start, all_cars)
