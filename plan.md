Car:
  - ID: (int)
  - Status: (WAITING_AT_INTERSECTION, WAITING_FOR_NEXT_CAR, TRAVELING)
  - Current position: (roadid, laneid, distance)
  - Route: list of (roadid)
  - Current destination: (roadid, distance)
  - Schedule: list of ((roadid, distance), arrival time, earliest leaving time) sorted by chronological order
  - Driver speed factor (float)
  - Max acceleration: (float)
  - Velocity: float

DISTANCES ARE FROM START OF LANE

Road:
  - id
  - ALWAYS one-directional
  - speed limit: (float)
  - Lanes: list(lane)
  - End intersection: intersectionID

Intersection:
  - ID
  - Outgoing roads: (roadid)
  - Schedule: list(map(R, Y, G))

Lane:
  - id
  - Cars: set (distnace, carid)


car.recomputeRoute():
  do naive BFS on routes

car.move():
  if distance to next intersection and yellow or red/car is less than stopping distance + margin:
    car.decelerate()
  elif not at max velocity:
    car.accelerate()
  increment current position by velocity

car.draw():
  draw the car

runSimulation():
  for tick in day:
    for car in cars:
      if tick is multiple of RECOMP_TIME:
        car.recomputeRoute()
      car.move()
      car.draw()