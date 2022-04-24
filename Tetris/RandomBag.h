#pragma once
#include "RandomGenerator.h"
#include <vector>

template<typename T>
class RandomBag : public RandomGenerator
{
private:
	std::vector<T> container;
	int indexOf(T item);
	void removeAt(int index);
	T valueAt(int index);
public:
	void insert(T item);
	bool removeOneOfItem(T item);
	bool removeAllOfItem(T item);
	int size();
	void empty();
	bool isInBag(T item);
	int countItem(T item);
	double itemChance(T item);
	T takeOneRandomOut(bool* ok = nullptr);
	T peekOneRandom(bool* ok = nullptr);
};

template<typename T>
int RandomBag<T>::indexOf(T item)
{
	auto it = std::find(this->container.begin(), this->container.end(), item);

	if (it != this->container.end())
	{
		int index = it - this->container.begin();
		return index;
	}

	return -1;
}

template<typename T>
void RandomBag<T>::removeAt(int index)
{
	this->container.erase(this->container.begin() + index);
}

template<typename T>
T RandomBag<T>::valueAt(int index)
{
	return this->container.at(index);
}

template<typename T>
inline void RandomBag<T>::insert(T item)
{
	this->container.push_back(item);
}

template<typename T>
inline bool RandomBag<T>::removeOneOfItem(T item)
{
	for (int i = 0; i < this->container.size; i++)
	{
		if (this->container[i] == item)
		{
			this->removeAt(i);
			return true;
		}
	}

	return false;
}

template<typename T>
inline bool RandomBag<T>::removeAllOfItem(T item)
{
	auto it = this->container.erase(std::remove_if(this->container.begin(), this->container.end(),
		[&item](const T& element) {
			return element == item;
		}));

	return it != this->container->end();
}

template<typename T>
inline int RandomBag<T>::size()
{
	return this->container.size();
}

template<typename T>
inline void RandomBag<T>::empty()
{
	this->container.clear();
}

template<typename T>
inline bool RandomBag<T>::isInBag(T item)
{
	return this->indexOf(item) != -1;
}

template<typename T>
inline int RandomBag<T>::countItem(T item)
{
	return std::count(this->container.begin(), this->container.end(), item);
}

template<typename T>
inline double RandomBag<T>::itemChance(T item)
{
	return static_cast<double>(this->countItem(item)) / static_cast<double>(this->size());
}

template<typename T>
inline T RandomBag<T>::takeOneRandomOut(bool* ok)
{
	if (this->container.size() == 0)
	{
		if (ok != nullptr)
		{
			*ok = false;
		}

		return T();
	}
	int random_index = this->getRandomInt(1, this->container.size()) - 1;
	T value = this->valueAt(random_index);
	this->removeAt(random_index);
	return value;
}

template<typename T>
inline T RandomBag<T>::peekOneRandom(bool* ok)
{
	if (this->container.size() == 0)
	{
		if (ok != nullptr)
		{
			*ok = false;
		}

		return T();
	}
	int random_index = this->getRandomInt(1, this->container.size()) - 1;
	T value = this->valueAt(random_index);
	return value;
}
