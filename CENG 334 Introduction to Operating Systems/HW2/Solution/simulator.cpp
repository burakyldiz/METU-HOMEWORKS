#include <iostream>
#include <vector>
#include <pthread.h>
#include <sstream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "WriteOutput.h" // Include WriteOutput functions
#include "helper.c" // Include helper functions and constants
#include "monitor.h" // Include monitor class for synchronization
#include <string>
#include <chrono>
#include <ctime>

// Forward declarations for classes
class NarrowBridge;
class Ferry;
class Crossroads;
class Car;

// Global variables for connectors and cars
std::vector<NarrowBridge> narrowBridges;
std::vector<Ferry> ferries;
std::vector<Crossroads> crossroads;
std::vector<Car> cars;



struct CarProperties {
    int travelTime;
    std::vector<std::tuple<char, int, int, int>> path; // Connector type, ID, from direction, to direction
};


// Connector classes definitions
class NarrowBridge: public Monitor {
public:

    
    Condition zero,one,zero1,one1,cv;
    int connectorID; // ID of the connector
    int currentDirection; // 0 or 1, indicating the current passing direction
    int maxWaitTime; // Maximum wait time for the opposing direction
    int travelTime; // Time to cross the bridge
    int passingCars = 0; // Number of cars currently passing
    int carsPassed =0;
    std::queue<int> direction0Queue;
    std::queue<int> direction1Queue;
    timespec now;
    timespec timeout;
    int isTimedOut;
    int timeFlag = 0;


    NarrowBridge(int travel, int maxWait, int connectorID) : zero(this), one(this), zero1(this), one1(this), cv(this),connectorID(connectorID)  , maxWaitTime(maxWait), travelTime(travel)  {}
    void addToQueue(int carID, int direction)
    
    {
        if (direction == 0)
        {
            direction0Queue.push(carID);
        }
        else
        {
            direction1Queue.push(carID);
        }
    }
    void setdirection(int passingCars, int direction)
    {
        
        if(!passingCars)
        {
            currentDirection = direction;

        }
    }
    void popFromQueue(int direction)
    {
        
         if (direction == 0) 
        {
        direction0Queue.pop();
  

        } 
        else 
        {
        direction1Queue.pop();

        }
    }
    bool isMyTurn(int carID, int direction)
    {
        
        if (direction == 0 && direction0Queue.front() == carID)
        {
            return true;
        }
        else if( direction == 1 && direction1Queue.front() == carID)
        {
            return true;
        }
        return false;
    }
    bool canChangeDirection(int direction) // pop before entering may cause problems
    {
        
        if (direction == 0)
        {
            if (passingCars == 0)
            {
                return true;
            }
        }
        else
        {
            if(passingCars == 0)
            {
                return true;
            }  
        }
        return false;  
    }
    void calculateTimeout()
    {
        
            clock_gettime(CLOCK_REALTIME, &now);
        // Convert maxWaitTime to nanoseconds and add to now.tv_nsec
            long long total_nsec = maxWaitTime * 1000000LL + now.tv_nsec;

            // Handle overflow of nanoseconds to seconds
            timeout.tv_sec = now.tv_sec + total_nsec / 1000000000;
            timeout.tv_nsec = total_nsec % 1000000000;
    }
    void changeDirection(int direction)
    {
        if (direction == 0)
        {
            currentDirection = 1;

            
        }
        else
        {
            currentDirection = 0;
            
        }
        
    }

    void Pass(int carID, int direction) {
        __synchronized__
        addToQueue(carID, direction);
        WriteOutput(carID, 'N', connectorID, ARRIVE);

        start:

        setdirection(passingCars, direction);

        if (currentDirection == direction) 
        {  
            if (passingCars != 0 && carsPassed && !timeFlag && isMyTurn(carID, direction))
            {  
                mutex.unlock();
                sleep_milli(PASS_DELAY);
                mutex.lock();
                goto check_turn;
            }
            
        check_turn:

            if (isMyTurn(carID, direction) && !timeFlag) 
            {  
                passingCars++;
                WriteOutput(carID, 'N', connectorID, START_PASSING);
                carsPassed = 1;
                popFromQueue(direction);
                one1.notifyAll();
                mutex.unlock();
                sleep_milli(travelTime);
                mutex.lock();
                WriteOutput(carID, 'N', connectorID, FINISH_PASSING);
                passingCars--;
                
                if (canChangeDirection(direction)) 
                {  
                    timeFlag = 0; 
                    carsPassed = 0;
                    zero.notifyAll();
                    one.notifyAll();
                }
                goto end;
            } 

            else 
            {  
                one1.wait();
                goto start;
            }
            
        } 

        else 
        {
            if (isMyTurn(carID, direction)) 
            {  
                calculateTimeout();
                isTimedOut = zero.timedwait(&timeout);

                if (isTimedOut == ETIMEDOUT) 
                {  
                    timeFlag = 1;
                }
                one.notifyAll();
                goto start;
            } 
            
            else 
            {
                one.wait();
                goto start;
            }
        }

    end:
        return;
    }   
};

class Ferry: public Monitor {
public:
    
    Condition one,zero,cv;
    int travelTime;
    int connectorID;
    int capacity;
    int maxWaitTime; // Maximum wait time for departure
    int ferries[2] = {0,0}; 
    bool ferryDeparted = false; // Flag indicating if the ferry has departed
    std::queue<int> direction0Ferry;
    std::queue<int> direction1Ferry;
    timespec now;
    timespec timeout0;
    timespec timeout1;
    int isTimedOut1;
    int isTimedOut0;

    Ferry(int travelTime, int maxWait, int cap, int connectorID)
        : one(this), zero(this), cv(this), travelTime(travelTime), capacity(cap), maxWaitTime(maxWait), connectorID(connectorID) {}
    void addToFerry(int carID, int direction)
    {
        if (direction == 0)
        {
            direction0Ferry.push(carID);
            ferries[direction]++;
        }
        else
        {
            direction1Ferry.push(carID);
            ferries[direction]++;
        }
    }
    // bool isFull(int direction)
    // {
    //     if(direction == 0 && ferry0 == capacity)
    //     {
    //         return true;
    //     }
    //     else if(direction == 1 && ferry1 == capacity)
    //     {
    //         return true;
    //     }
    //     return false;
    // }
    // void popOne(int direction)
    // {
    //     int i;
    //     if(direction == 0)
    //     {
            
    //         direction0Ferry.pop();
    //          ferry0--;
            
    //     }
    //     else
    //     {
            
    //         direction1Ferry.pop();
    //         ferry1--;
                
    //     }
    // }
    void calculateTimeout(int direction)
    {
        if(direction == 0)
        {    
            clock_gettime(CLOCK_REALTIME, &now);
        // Convert maxWaitTime to nanoseconds and add to now.tv_nsec
            long long total_nsec = maxWaitTime * 1000000LL + now.tv_nsec;

            // Handle overflow of nanoseconds to seconds
            timeout0.tv_sec = now.tv_sec + total_nsec / 1000000000;
            timeout0.tv_nsec = total_nsec % 1000000000;
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &now);
        // Convert maxWaitTime to nanoseconds and add to now.tv_nsec
            long long total_nsec = maxWaitTime * 1000000LL + now.tv_nsec;

            // Handle overflow of nanoseconds to seconds
            timeout1.tv_sec = now.tv_sec + total_nsec / 1000000000;
            timeout1.tv_nsec = total_nsec % 1000000000;
        }
    }
    bool isFirstCar(int carID, int direction)
    {
        if(direction == 0)
        {
            return true;
        }
        else if(direction == 1)
        {
            return true;
        }
        return false;
    }
    void popAll(int direction)
    {
        if(direction == 0)
        {
            while(!direction0Ferry.empty())
            {
                direction0Ferry.pop();
            }
        }
        else
        {
            while(!direction1Ferry.empty())
            {
                direction1Ferry.pop();
            }
        }
    }
    void Pass(int carID, int direction) 
    {
       
        __synchronized__
        WriteOutput(carID, 'F', connectorID, ARRIVE);
        addToFerry(carID, direction);

        if(ferries[direction] == 1)
        {
                calculateTimeout(direction);
                if(direction == 0)
                {
                    isTimedOut0 = zero.timedwait(&timeout0);
                    if(isTimedOut0 == ETIMEDOUT)
                    {
                        ferries[direction] = 0;
                        zero.notifyAll();
                    }
                    
                    
                    
                }
                else
                {
                    isTimedOut1 = one.timedwait(&timeout1);
                    if(isTimedOut1 == ETIMEDOUT)
                    {
                        ferries[direction] = 0;
                        one.notifyAll();
                    }
                    
                }
        }
        else if(ferries[direction] < capacity )
        {
            (direction == 1 ? one : zero).wait();
        }
        else
        {
            ferries[direction] = 0;
            (direction == 1 ? one : zero).notifyAll();
        }
            WriteOutput(carID, 'F', connectorID, START_PASSING);
            mutex.unlock();
            sleep_milli(travelTime);
            mutex.lock();
            WriteOutput(carID, 'F', connectorID, FINISH_PASSING);
    }
};

class Crossroads: public Monitor{
public:
    Condition zero,one,zero1,one1,cv;
    int connectorID; // ID of the connector
    int currentDirection; // 0 or 1, indicating the current passing direction
    int maxWaitTime; // Maximum wait time for the opposing direction
    int travelTime; // Time to cross the bridge
    int passingCars = 0; // Number of cars currently passing
    std::queue<int> direction0Queue;
    std::queue<int> direction1Queue;
    std::queue<int> direction2Queue;
    std::queue<int> direction3Queue;
    timespec now;
    timespec timeout;
    int isTimedOut;
    int timeFlag = 0;
    int carsPassed = 0;

    Crossroads(int travel, int maxWait, int connectorID) : zero(this), one(this), zero1(this), one1(this), cv(this),connectorID(connectorID)  , maxWaitTime(maxWait), travelTime(travel)  {}

    void addToQueue(int carID, int direction)
    
    {
        if (direction == 0)
        {
            direction0Queue.push(carID);
        }
        else if(direction == 1)
        {
            direction1Queue.push(carID);
        }
        else if(direction == 2)
        {
            direction2Queue.push(carID);
        }
        else if(direction == 3)
        {
            direction3Queue.push(carID);
        }
    }
    void setdirection(int passingCars, int direction)
    {
        
        if(!passingCars)
        {
            currentDirection = direction;

        }
    }
    void popFromQueue(int direction)
    {
        
         if (direction == 0) 
        {
        direction0Queue.pop();
        } 
        else if(direction == 1)
        {
        direction1Queue.pop();
        }
        else if(direction == 2)
        {
        direction2Queue.pop();
        }
        else if(direction == 3)
        {
        direction3Queue.pop();
        }
    }
    bool isMyTurn(int carID, int direction)
    {
        
        if (direction == 0 && direction0Queue.front() == carID)
        {
            return true;
        }
        else if( direction == 1 && direction1Queue.front() == carID)
        {
            return true;
        }
        else if( direction == 2 && direction2Queue.front() == carID)
        {
            return true;
        }
        else if( direction == 3 && direction3Queue.front() == carID)
        {
            return true;
        }
        return false;
    }
    bool canChangeDirection(int direction) // pop before entering may cause problems
    {
        
        if (direction == 0)
        {
            if (passingCars == 0)
            {
                return true;
            }
        }
        else
        {
            if(passingCars == 0)
            {
                return true;
            }  
        }
        return false;  
    }
    void calculateTimeout()
    {
        
            clock_gettime(CLOCK_REALTIME, &now);
        // Convert maxWaitTime to nanoseconds and add to now.tv_nsec
            long long total_nsec = maxWaitTime * 1000000LL + now.tv_nsec;

            // Handle overflow of nanoseconds to seconds
            timeout.tv_sec = now.tv_sec + total_nsec / 1000000000;
            timeout.tv_nsec = total_nsec % 1000000000;
    }
    void changeDirection(int direction)
    {
        if (direction == 0)
        {
            currentDirection = 1;

            
        }
        else
        {
            currentDirection = 0;
            
        }
        
    }
    void findNextDirection()
    {
        /* Next active direction is determined by incrementing numbers. Direction details will be
explained at the end of this section.
The simulation will be subjected to these constraints.
1. Directions for car path determined by two numbers representing (f rom, to) for connec-
tors. For narrow bridges and ferries, it can either be (0, 1) or (1, 0). For crossroads, it
can be one of the sixteen values in the form of ([0, 3], [0, 3]). However, when considering
lanes, only the from value is considered. This means that crossroads have four passing
lanes */
        if(currentDirection == 0)
        {
            if(direction1Queue.size() > 0)
            {
                currentDirection = 1;
                return;
            }
            else if(direction2Queue.size() > 0)
            {
                currentDirection = 2;
                return;
            }
            else if(direction3Queue.size() > 0)
            {
                currentDirection = 3;
                return;
            }
        }
        else if(currentDirection == 1)
        {
            if(direction2Queue.size() > 0)
            {
                currentDirection = 2;
                return;
            }
            else if(direction3Queue.size() > 0)
            {
                currentDirection = 3;
                return;
            }
            else if(direction0Queue.size() > 0)
            {
                currentDirection = 0;
                return;
            }
        }
        else if(currentDirection == 2)
        {
            if(direction3Queue.size() > 0)
            {
                currentDirection = 3;
                return;
            }
            else if(direction0Queue.size() > 0)
            {
                currentDirection = 0;
                return;
            }
            else if(direction1Queue.size() > 0)
            {
                currentDirection = 1;
                return;
            }
        }
        else if(currentDirection == 3)
        {
            if(direction0Queue.size() > 0)
            {
                currentDirection = 0;
                return;
            }
            else if(direction1Queue.size() > 0)
            {
                currentDirection = 1;
                return;
            }
            else if(direction2Queue.size() > 0)
            {
                currentDirection = 2;
                return;
            }
        }
    }
    void Pass(int carID, int direction) {
        __synchronized__
       addToQueue(carID, direction);
       WriteOutput(carID, 'C', connectorID, ARRIVE);

        start:
            setdirection(passingCars, direction);

            if (currentDirection == direction) {  
                if (passingCars != 0) {  
                    mutex.unlock();
                    sleep_milli(PASS_DELAY);
                    mutex.lock();
                } 
                if (isMyTurn(carID, direction) && !timeFlag) {  // öndeyim timeout yok
                    passingCars++;
                    WriteOutput(carID, 'C', connectorID, START_PASSING); 
                    carsPassed = 1;
                    popFromQueue(direction); 
                    one1.notifyAll(); 
                    mutex.unlock();
                    sleep_milli(travelTime); 
                    mutex.lock();
                    WriteOutput(carID, 'C', connectorID, FINISH_PASSING); 
                    passingCars--;
                    
                    if (canChangeDirection(direction)) { // değişebilir mi
                        timeFlag = 0; 
                        carsPassed = 0;
                        zero.notifyAll();
                        one.notifyAll();
                        findNextDirection(); // direction + 1 yer varsa
                    }
                    goto end; 
                } else {  
                    one1.wait();
                    goto start; // başa dön
                }
            } else {
                if (isMyTurn(carID, direction)) {  // tersten gelen ilk
                    calculateTimeout(); // setle
                    isTimedOut = zero.timedwait(&timeout);
                    if (isTimedOut == ETIMEDOUT) {  // if timeout
                        timeFlag = 1; // timeouto ldu
                    }
                    one.notifyAll(); 
                    goto start; // başa dön
                } else {
                    one.wait(); // sıra bende değilse bekle
                    goto start; // başa dön
                }
            }

        // bitir
        end:
            return;

    }   
};

// Car class representing a car in the simulation
class Car {
public:
    int carID;
    int travelTimeCar; // Travel time between connectors
    std::vector<std::tuple<char, int, int, int>> path; // Connector type, ID, from-to directions
    std::string currentAction; // Current action of the car (e.g., TRAVEL, ARRIVE)

    Car(int id, int travel) : carID(id), travelTimeCar(travel) {}

    void Simulate() {
        for (const auto& step : path) {
            char connectorType = std::get<0>(step);
            int connectorID = std::get<1>(step);
            int fromDirection = std::get<2>(step);
            int toDirection = std::get<3>(step);


            WriteOutput(carID, connectorType, connectorID, TRAVEL);

            sleep_milli(travelTimeCar); // Travel time between connectors

            switch (connectorType) {
                case 'N':

                    narrowBridges[connectorID].Pass(carID, toDirection);
                    break;
                case 'F':

                    ferries[connectorID].Pass(carID, toDirection);
                    break;
                case 'C':

                    crossroads[connectorID].Pass(carID, fromDirection);
                    break;
            }
        }
    }
};

void* threadRoutine(void* arg) {
    Car* car = static_cast<Car*>(arg);
    car->Simulate();
    return nullptr;
}

int main() {


    std::string line;
    std::istringstream iss;

    // Read number of narrow bridges
    std::getline(std::cin, line);
    iss.str(line);
    int numNarrowBridges;
    if (!(iss >> numNarrowBridges)) {
        
        return 1;
    }
    
    iss.clear();

    for (int i = 0; i < numNarrowBridges; i++) {
        std::getline(std::cin, line);
        iss.str(line);
        int travelTime, maxWaitTime;
        if (!(iss >> travelTime >> maxWaitTime)) {
            
            continue;
        }
        
        iss.clear();
        narrowBridges.emplace_back(travelTime, maxWaitTime,i);
    }

    // Read number of ferries
    std::getline(std::cin, line);
    iss.str(line);
    int numFerries;
    if (!(iss >> numFerries)) {
        
        return 1;
    }
    
    iss.clear();

    for (int i = 0; i < numFerries; i++) {
        std::getline(std::cin, line);
        iss.str(line);
        int travelTime, maxWaitTime, capacity;
        if (!(iss >> travelTime >> maxWaitTime >> capacity)) {
            
            continue;
        }
        
        iss.clear();
        ferries.emplace_back(travelTime, maxWaitTime, capacity, i);
    }

    // Read number of crossroads
    std::getline(std::cin, line);
    iss.str(line);
    int numCrossroads;
    if (!(iss >> numCrossroads)) {
        
        return 1;
    }
    
    iss.clear();

    for (int i = 0; i < numCrossroads; i++) {
        std::getline(std::cin, line);
        iss.str(line);
        int travelTime, maxWaitTime;
        if (!(iss >> travelTime >> maxWaitTime)) {
            
            continue;
        }
        
        iss.clear();
        crossroads.emplace_back(travelTime, maxWaitTime,i);
    }

    // Read number of cars
   std::getline(std::cin, line);
    iss.str(line);
    iss.clear(); // Reset the stream state before reading
    int numCars;
    if (!(iss >> numCars)) {
        
        return 1;
    }
    iss.clear(); // Clear after reading

    for (int i = 0; i < numCars; i++) {
        if (!std::getline(std::cin, line)) {
            
            continue; // Skip to next iteration if line read fails
        }
        iss.str(line);
        iss.clear(); // Clear and reset the stream with new line data

        int carTravelTime, pathLength;
        if (!(iss >> carTravelTime >> pathLength)) {
            
            continue; // Skip this car if reading fails
        }
        

        CarProperties carProps;
        carProps.travelTime = carTravelTime;

        if (!std::getline(std::cin, line)) {
            
            continue; // Skip to next car if path line read fails
        }
        iss.str(line);
        iss.clear(); // Reset the stream for path data

        for (int j = 0; j < pathLength; j++) {
            std::string connectorTypeAndID;
            int fromDirection, toDirection;
            if (!(iss >> connectorTypeAndID >> fromDirection >> toDirection)) {
                
                continue; // Continue to next path segment if current one fails
            }

            char connectorType = connectorTypeAndID[0];
            int connectorID = std::stoi(connectorTypeAndID.substr(1)); // Convert the ID part

            
            carProps.path.emplace_back(connectorType, connectorID, fromDirection, toDirection);
        }

        cars.emplace_back(i, carProps.travelTime);
        cars.back().path = carProps.path;
    }
    // Array to hold thread IDs
    pthread_t threads[cars.size()];

    // Create threads for each car
    InitWriteOutput();
    for (size_t i = 0; i < cars.size(); ++i) {
        pthread_create(&threads[i], nullptr, threadRoutine, &cars[i]);
    }

    // Join threads
    for (size_t i = 0; i < cars.size(); ++i) {
        pthread_join(threads[i], nullptr);
     }


    return 0;
}