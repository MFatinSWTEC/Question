#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include <ctime>
#include <iostream>
#include <array>
#include <thread>
#include <functional>
#include <vector>
#include <random>
#include <unordered_map>
#include <map>
#include <mutex>
#include <atomic>


const int screenSize = 800;
const int pointSize = 20;

enum class CellState {
    EMPTY,
    COLLISION,
    PLANE
};

enum class CellAnimation {
    APPEAR,
    DISAPPEAR
};

class Plane {
public:
    using Position = std::pair<int, int>;
    using Speed = std::pair<int, int>;

    Plane() = default;

    Plane(Position position, Speed speed, size_t id, std::vector<float> color) : speed{ speed }, position{ position }, id{ id }, color{ color } {}

    size_t getID() {
        return id;
    }

    Speed getSpeed() {
        return speed;
    }

    Position getPosition() {
        return position;
    }

    void setPosition(Position newPosition) {
        position = newPosition;
    }

    int getSize() {
        return size;
    }

    void setSize(int newSize) {
        size = newSize;
    }

    std::vector<float>& getColor() {
        return color;
    }

    void setColor(std::vector<float> newColor) {
        color = newColor;
    }

private:
    
    size_t id = -1;
    Position position;
    int size = 20;
    const Speed speed;
    std::vector<float> color;
};

class PlanesControl {
public:

    using Position = std::pair<int, int>;
    using Speed = std::pair<int, int>;
    size_t lastID = 1;

    std::random_device rd;
    std::mt19937 randomGenerator{ rd() };


    bool addPlane(Position position, Speed speed) {
        if (speed.first == 0 && speed.second == 0) {
            return false;
        }
        for(auto & plane: planes) {
            if (plane.second.getPosition() == position) {
                return false;
            }
        }
        
        std::uniform_real_distribution<> distibutor(0.1, 0.9);

        auto color = { 
            static_cast<float>(distibutor(randomGenerator)),
            static_cast<float>(distibutor(randomGenerator)),
            static_cast<float>(distibutor(randomGenerator)) 
        };
        planes.insert(std::make_pair<int, Plane>(int(lastID), Plane(position, speed, lastID++, color)));
        return true;
    }

    bool addRandomPlane() {
        std::uniform_int_distribution<> bool_distrib(0, 1);
        std::uniform_int_distribution<> pos_distrib(0, getScreenSize() - 1);
        std::uniform_int_distribution<> speed_distrib(-1, 1);


        int posX = bool_distrib(randomGenerator) == 0 ? 0 : getScreenSize() - 1;
        int posY = pos_distrib(randomGenerator);
        if (bool_distrib(randomGenerator)) {
            std::swap(posX, posY);
        }
        
        int speedX = speed_distrib(randomGenerator);
        int speedY = speed_distrib(randomGenerator);


        if ((posX == 0 || posX == getScreenSize() - 1) && speedX == 0  || (posY == 0 || posY == getScreenSize() - 1) && speedY == 0) {
            return false;
        }
        
        return addPlane(std::make_pair(posX, posY), std::make_pair(speedX, speedY));
    }

    void deletePlaneByID(size_t id) {
        planes.erase(id);
    }

    void deleteCollisionPlanes(std::vector<std::vector<int>>& palaneIDs) {
        for (auto& elem : palaneIDs) {
            for (auto& el : elem) {
                deletePlaneByID(el);
            }
        }
    }

    void movePlanes() {
        for (auto& plane: planes) {
            auto position = plane.second.getPosition();

            auto speed = plane.second.getSpeed();
            auto newPosition = std::make_pair<int, int>(
                position.first + speed.first,
                position.second + speed.second);
            plane.second.setPosition(newPosition);

            auto position1 = plane.second.getPosition();
        }
    }
    
    void deletePlanesOutOfRadarRange() {
        for (auto it = planes.begin(); it != planes.end();) {
            auto position = it->second.getPosition();
            if (!isValidePosition(position)) {
                it = planes.erase(it);
                continue;
            }
            ++it;
        }
    }
    
    std::unordered_map<size_t, Plane>& getPlanes() {
        return planes;
    }

    std::unordered_map<size_t, Plane> getCopyPlanes() {
        return planes;
    }

    bool isValidePosition(Position position) {
        return 0 <= position.first && position.first < maxPositionX &&
            0 <= position.second && position.second < maxPositionY;
    }

    int getScreenSize() {
        return maxPositionX;
    }

private:
    const size_t maxPositionX = screenSize / pointSize;
    const size_t maxPositionY = screenSize / pointSize;

    std::unordered_map<size_t, Plane> planes;
};



class Drawer {
public:
    const size_t delayAnimation = 30;

    void disappear(PlanesControl& pc, std::mutex& mtx) {
        playAnimation(CellAnimation::DISAPPEAR, pc.getPlanes(), mtx);
    }

    void appear(PlanesControl& pc, std::mutex& mtx) {
        playAnimation(CellAnimation::APPEAR, pc.getPlanes(), mtx);
    }
private:
    void playAnimation(CellAnimation animationType, std::unordered_map<size_t, Plane>& planes, std::mutex& mtx) {
        int basicShift = pointSize / 2;
        for (int k = 0; k < pointSize; ++k) {

            for (auto& plane: planes) {
               
                std::unique_lock<std::mutex> lock(mtx);
                showAnimation(animationType, plane.second);

            }

            std::this_thread::sleep_for(std::chrono::milliseconds(delayAnimation));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void showAnimation(CellAnimation animation, Plane& plane) {
        int change = animation == CellAnimation::APPEAR ? 1 : -1;
        int newCellSize = change + plane.getSize();
        if (change > 0 && newCellSize == 1) {
            newCellSize = 6;
        }
        if (!(0 <= newCellSize && newCellSize <= pointSize)) {
            return;
        }
        plane.setSize(newCellSize);
    }

};

class Collision {
public:
    struct pair_hash;

    using Position = std::pair<int, int>;
    PlanesControl pc;

    std::vector<std::vector<int>> getCollision(PlanesControl& pc) {
        std::unordered_map<size_t, Plane>& planeID = pc.getPlanes();
        std::unordered_map<Position, std::vector<int>, pair_hash> pos;
        for (auto& id_plane : planeID) {
            pos[id_plane.second.getPosition()].emplace_back(id_plane.first);
        }

        return normalizeData(pos);
    }
private:

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1, T2>& pair) const {
            auto h1 = std::hash<T1>{}(pair.first);
            auto h2 = std::hash<T2>{}(pair.second);
            return h1 + h2 * 160;
        }
    };

    std::vector<std::vector<int>> normalizeData(std::unordered_map<Position, std::vector<int>, pair_hash>& pos) {
        std::vector<std::vector<int>> result;
        for (auto& el : pos) {
            if (el.second.size() > 1) {
                result.emplace_back(el.second);
            }
        }
        return result;
    }
};


class FutureCollision {
public:
    using Position = std::pair<int, int>;

    using ID_Position = std::unordered_map<size_t, std::vector<Position>>;

    std::pair<ID_Position, std::vector<Position>> getFutureCollision(PlanesControl& pc) {
        ID_Position id_position;
        std::vector<Position> pos_collisions;
        
        std::unordered_map<size_t, Plane> planes = pc.getCopyPlanes();

        while (!planes.empty()) {
            movePlanes(planes);
            auto collisions = getCollision(planes);

            for (auto& collision : collisions) {
                pos_collisions.emplace_back(collision.first);

                for (auto& id : collision.second) {
                    planes.erase(id);
                }
            }

            for (auto it = planes.begin(); it != planes.end();) {
                if (!pc.isValidePosition(it->second.getPosition())) {
                    it = planes.erase(it);
                    continue;
                }
                ++it;
            }

            for (auto& plane : planes) {
                id_position[plane.first].emplace_back(plane.second.getPosition());
            }
        }
        return { id_position , pos_collisions };
    }

private:


    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1, T2>& pair) const {
            auto h1 = std::hash<T1>{}(pair.first);
            auto h2 = std::hash<T2>{}(pair.second);
            return h1 + h2 * 160;
        }
    };

    void movePlanes(std::unordered_map<size_t, Plane>& planes) {
        for (auto& plane : planes) {
            auto position = plane.second.getPosition();

            auto speed = plane.second.getSpeed();
            auto newPosition = std::make_pair<int, int>(
                position.first + speed.first,
                position.second + speed.second);
            plane.second.setPosition(newPosition);
        }
    }

    std::vector<std::pair<Position, std::vector<int>>> getCollision(std::unordered_map<size_t, Plane>& planeID) {
        std::unordered_map<Position, std::vector<int>, pair_hash> pos;
        for (auto& id_plane : planeID) {
            pos[id_plane.second.getPosition()].emplace_back(id_plane.first);
        }

        return normalizeData(pos);
    }

    std::vector<std::pair<Position, std::vector<int>>> normalizeData(std::unordered_map<Position, std::vector<int>, pair_hash>& pos) {
        std::vector<std::pair<Position, std::vector<int>>> result;
        for (auto& el : pos) {
            if (el.second.size() > 1) {
                result.emplace_back(el.first, el.second);
            }
        }
        return result;
    }
};


class MainModule {
public:
    PlanesControl pc;
    Drawer d;
    Collision collision;
    FutureCollision futCollision;

    std::atomic_bool draw = false;


    void routine(std::mutex& mtx) {
   //     pc.addPlane(std::make_pair<int, int>(0, 0), std::make_pair<int, int>(1, 1));
   //     pc.addPlane(std::make_pair<int, int>(10, 10), std::make_pair<int, int>(-1, -1));
   //     pc.addPlane(std::make_pair<int, int>(10, 5), std::make_pair<int, int>(-1, 0));

    //    pc.addPlane(std::make_pair<int, int>(20, 20), std::make_pair<int, int>(-1, -1));
       // pc.addPlane(std::make_pair<int, int>(9, 0), std::make_pair<int, int>(-1, 0));
        
        pc.addPlane(std::make_pair<int, int>(0, 36), std::make_pair<int, int>(1, 0));
        pc.addPlane(std::make_pair<int, int>(0, 0), std::make_pair<int, int>(1, 1));
        while (true) {

            d.disappear(pc, mtx);

            std::unique_lock<std::mutex> lock(mtx);
            pc.movePlanes();
            auto coll = collision.getCollision(pc);
            pc.deleteCollisionPlanes(coll);
            pc.deletePlanesOutOfRadarRange();
            lock.unlock();

            d.appear(pc, mtx);

            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            lock.lock();
            for (int i = 0; i < 50; ++i) {
                pc.addRandomPlane();
            }
            lock.unlock();
        }
    }

};

void drawLines() {
    for (size_t x = 0; x <= screenSize; x += pointSize) {
        glBegin(GL_LINES);

        //glColor3f(0, 0, 0);
        glColor4f(0.1, 0.2, 0.1, 0.2);

        glVertex2f(x, screenSize);
        glVertex2f(x, 0);

        glVertex2f(screenSize, x);
        glVertex2f(0, x);
        glEnd();
    }
}

void drawBorder() {
    int pos = pointSize * (screenSize / pointSize - 1);

    glBegin(GL_LINES);

    glColor3f(0, 0, 0);

    glVertex2f(pointSize, pointSize);
    glVertex2f(pointSize, pos);

    glVertex2f(pointSize, pos);
    glVertex2f(pos, pos);

    glVertex2f(pos, pos);
    glVertex2f(pos, pointSize);

    glVertex2f(pos, pointSize);
    glVertex2f(pointSize, pointSize);

    glEnd();
}

void drawPlanes(std::unordered_map<size_t, Plane>& planes) {
    for (auto& plane : planes) {
        if (plane.second.getSize() <= 0) {
            continue;
        }
        auto position = plane.second.getPosition();
        auto posX = position.first * pointSize + 10, posY = position.second * pointSize + 10;
        auto color = plane.second.getColor();

        glPointSize(plane.second.getSize());
        glBegin(GL_POINTS);
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(posX, posY);
        glEnd();
    }
}

void drawPath(std::unordered_map<size_t, Plane>& planes, std::unordered_map<size_t, std::vector<std::pair<int, int>>>& futPos) {
    for (auto& pos : futPos) {

        for (auto& coord : pos.second) {
            auto posX = coord.first * pointSize + 10, posY = coord.second * pointSize + 10;
            auto color = planes[pos.first].getColor();

            glPointSize(5);
            glBegin(GL_POINTS);
            glColor4f(color[0], color[1], color[2], 0.7);
            glVertex2f(posX, posY);
            glEnd();
        }
    }
}

void drawFutureCollision(std::vector<std::pair<int, int>>& futCollision) {
    for (auto& collisionPos : futCollision) {
        auto posX = collisionPos.first * pointSize + 10, posY = collisionPos.second * pointSize + 10;

        glPointSize(10);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0, 0);
        glVertex2f(posX, posY);
        glEnd();
        glPopMatrix();
    }
}

void drawField(MainModule& c, std::mutex& mtx) {
    //glLineWidth(20);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    drawLines();
    drawBorder();

    std::unique_lock<std::mutex> lock(mtx);
    auto planes = c.pc.getCopyPlanes();
    // std::pair<ID_Position, std::vector<Position>>
    auto futureRes = c.futCollision.getFutureCollision(c.pc);
    lock.unlock();


    drawPlanes(planes);
    drawPath(planes, futureRes.first);
    drawFutureCollision(futureRes.second);
}




int main() {
	if (!glfwInit())
		return -1;
	
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenSize, screenSize, "Alorithm", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

   
   
    
    //CellField c;
    //std::thread graphic(draw, std::ref(c));
    //graphic.detach();

    MainModule m;
    std::mutex mtx;
    std::thread graphic(&MainModule::routine, std::ref(m), std::ref(mtx));
    graphic.detach();

    while (!glfwWindowShouldClose(window)) {



        glClearColor(1, 1, 1, 0); // 0.0f - 1.0f
        glClear(GL_COLOR_BUFFER_BIT);


        glLoadIdentity();
        glScalef(2.0 / screenSize, 2.0 / screenSize, 1);
        glTranslatef(-screenSize * 0.5, -screenSize * 0.5, 0);

        
        
        drawField(m, mtx);
        //c.disappear();

        // Display 
        glfwSwapBuffers(window);

        // Pollevents
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// vector out of range ?
