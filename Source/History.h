#pragma once
#include <vector>

template <typename T>
class History 
{
public:
	History(const size_t max_steps) : max_steps(max_steps) {
		history.reserve(max_steps);

		for (size_t i = 0; i < max_steps; ++i)
			history.emplace_back(nullptr);
	}

	~History() {
		history.clear(); // Doesn't hold actual data so there is nothing to delete
	}

	void Add(T* node) {
		if (history_size > 0 && current != tail) // New Node when current_history is not tail
		{
			tail = (current < max_steps - 1) ? current : 0;
			history_size = (current >= head) ? current - head + 1 : max_steps - (head - current) + 1;
		}

		if (history_size >= max_steps)
		{
			(head < max_steps - 1) ? head++ : head = 0;
			(tail < max_steps - 1) ? tail++ : tail = 0;
		}
		else
		{
			tail++;
			history_size++;
		}
		current = tail;
		history[tail] = node;
	}

	void Forward() {
		if (current == tail)
			return;

		(current == max_steps - 1) ? current = 0 : current++;
	}

	void Backward() {
		if (current == head)
			return;

		(current == 0) ? current = max_steps - 1 : current--;
	}

	T* GetCurrent() const { return history[current]; }

	bool IsForwardAvail() const { return current != tail; }
	bool IsBackwardAvail() const { return current != head; }

private:
	std::vector<T*> history;
	size_t max_steps = 0;

	int head = 0;
	int tail = -1;
	int current = 0;
	int history_size = 0;
};