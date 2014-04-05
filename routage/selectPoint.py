from parse import *
from diaspora import main_diaspora

GAIN_MULT = 5

def trivial(cars, inters, rues, time, nvehic, start, all_cars):
    points = []
    points.append((48.898033, 2.346924)) ## haut
    points.append((48.821010, 2.356194)) ## bas
    points.append((48.861005, 2.268647)) ## gauche
    points.append((48.859875, 2.408379)) ## droite
    points.append(())
    points.append(())
    points.append(())
    points.append(())
    


    print(points[0])
    ##for inter in inters:




cars, inters, rues, time, nvehic, start, all_cars = parse(sys.stdin)
trivial(cars, inters, rues, time, nvehic, start, all_cars)
