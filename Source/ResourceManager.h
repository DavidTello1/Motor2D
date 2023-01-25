#pragma once
#include "Globals.h"

#include <array>

class BaseResourceManager
{
public:
	virtual ~BaseResourceManager() = default;
};

template<typename T>
class ResourceManager : public BaseResourceManager
{
public:
	ResourceManager() {};

	int GetSize() const { 
		return count;
	}

	bool Add(size_t index, T& resource) {
		if (index >= MAX_RESOURCES)
			return false;

		data[count] = resource;
		++count;
		return true;
	}

	bool Remove(size_t index) {
		if (index >= count)
			return false;

		--count;
		T resource = data[count];
		data[index] = resource;
		return true;
	}

	T& GetResource(size_t index) {
		//if (index >= count)
		//	return nullptr;

		return data[index];
	}

	const T& GetResource(size_t index) const {
		//if (index >= count)
		//	return nullptr;

		return data[index];
	}

	//std::vector<T>& GetAllResources() { // *** RETURN VECTOR WITH PACKED DATA
	//	return data;
	//}

	//const std::vector<T>& GetAllResources() const { // *** RETURN VECTOR WITH PACKED DATA
	//	return data;
	//}

	bool SetResource(size_t index, T& resource) {
		if (index >= count)
			return false;

		data[index] = resource;
		return true;
	}

private:
	static constexpr int MAX_RESOURCES = 1000;

	std::array<T, MAX_RESOURCES> data;
	int count = 0;
};
