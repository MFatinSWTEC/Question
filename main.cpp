#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>


class Plane {
public:
	// X Y
	using speed_type = std::pair<int, int>;

	Plane(std::pair<int, int> coord);

	void movePlane() {}

private:
	uint64_t id;
	speed_type speed;
};

class Planess {
public:
	using speed_type = std::pair<int, int>;
	using coord_type = std::pair<int, int>;
	using planess_type = std::unordered_map<coord_type, std::vector<Plane>>;

	void add_plane(speed_type speed, coord_type coord) {
		//planes[coord].emplace_back(speed);
	}

	planess_type get_planess() {}

	void movePlaness() {}

private:
	planess_type planes;
};

class Collision {
public:
	using collisionList = int;

	collisionList calculateCollisionInMoment(std::shared_ptr<Planess> planess) {}
};

class CollisionInFuture {
public:
	using collisionList = int;

	collisionList calculateCollisionInFuture(std::shared_ptr<Planess> planess) {}
};

class MainModule {
	void work() {

	}
};

enum class OutputMode {
	CONSOLE,
	FILE
};

class OutputModule {
	using collisionList = int;
	
	uint64_t displayX;
	uint64_t displayY;

	void print(collisionList, OutputMode, std::string file_path="") {

	}
};
