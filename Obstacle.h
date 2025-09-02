#pragma once
class Obstacle
{
public:
	Obstacle() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr_distance(7, 15), distr_size(1, 3);

		size = distr_size(gen);
		distance = distr_distance(gen);
	}

	unsigned short get_pos() {
		return position;
	}

	unsigned short get_size() {
		return size;
	}

	unsigned short get_dist() {
		return distance;
	}

	unsigned short get_passed() {
		return passed;
	}

	void pos_decrement() {
		position--;
	}

	void passed_increment() {
		passed++;
	}

	bool distance_check() const
	{
		if (size == 0) return true;
		if (distance == passed) return true;
		return false;
	}

private:
	unsigned short position = 57, distance, passed = 0, size = 0;
};