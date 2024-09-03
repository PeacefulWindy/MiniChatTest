#pragma once
#include<queue>
#include<msg/msg.hpp>
#include<mutex>

template<class T>
class ThreadQueue
{
public:
	ThreadQueue();
	virtual ~ThreadQueue();

public:
	void lock();
	void unlock();

public:
	void push(T value);
	T getFront()const;
	void pop();
	size_t getSize()const;

private:
	std::queue<T> mQueue;
	std::mutex mLock;
};

template<class T>
inline ThreadQueue<T>::ThreadQueue()
{
	
}

template<class T>
inline ThreadQueue<T>::~ThreadQueue()
{

}

template<class T>
inline void ThreadQueue<T>::lock()
{
	this->mLock.lock();
}

template<class T>
inline void ThreadQueue<T>::unlock()
{
	this->mLock.unlock();
}

template<class T>
inline void ThreadQueue<T>::push(T value)
{
	this->mQueue.push(value);
}

template<class T>
inline T ThreadQueue<T>::getFront()const
{
	return this->mQueue.front();
}

template<class T>
inline void ThreadQueue<T>::pop()
{
	this->mQueue.pop();
}

template<class T>
inline size_t ThreadQueue<T>::getSize()const
{
	return this->mQueue.size();
}
