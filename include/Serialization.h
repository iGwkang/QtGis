#pragma once

/**
 * @file Serialization.h
 * @brief C++标准库的序列化和反序列化
 * @note C++标准库的流处理效率很低，不要频繁使用
 * @author gwk
 * @date 2019-12-7 14:09:07
 */

#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include <deque>
#include <sstream>

static std::iostream & operator<<(std::iostream & in, const char a)
{
	in.write(&a, sizeof(char));
	return in;
}

static std::iostream & operator>>(std::iostream & out, char &a)
{
	out.read(&a, sizeof(char));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const unsigned char a)
{
	in.write((const char *)&a, sizeof(unsigned char));
	return in;
}

static std::iostream & operator>>(std::iostream & out, unsigned char &a)
{
	out.read((char *)&a, sizeof(unsigned char));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const bool a)
{
	in.write((const char *)&a, sizeof(bool));
	return in;
}

static std::iostream & operator>>(std::iostream & out, bool &a)
{
	out.read((char *)&a, sizeof(bool));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const short a)
{
	in.write((const char *)&a, sizeof(short));
	return in;
}

static std::iostream & operator>>(std::iostream & out, short &a)
{
	out.read((char *)&a, sizeof(short));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const unsigned short a)
{
	in.write((const char *)&a, sizeof(unsigned short));
	return in;
}

static std::iostream & operator>>(std::iostream & out, unsigned short &a)
{
	out.read((char *)&a, sizeof(unsigned short));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const int a)
{
	in.write((const char *)&a, sizeof(int));
	return in;
}

static std::iostream & operator>>(std::iostream & out, int &a)
{
	out.read((char *)&a, sizeof(int));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const unsigned a)
{
	in.write((const char *)&a, sizeof(unsigned));
	return in;
}

static std::iostream & operator>>(std::iostream & out, unsigned &a)
{
	out.read((char *)&a, sizeof(unsigned));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const float a)
{
	in.write((const char *)&a, sizeof(float));
	return in;
}

static std::iostream & operator>>(std::iostream & out, float &a)
{
	out.read((char *)&a, sizeof(float));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const double a)
{
	in.write((const char *)&a, sizeof(double));
	return in;
}

static std::iostream & operator>>(std::iostream & out, double &a)
{
	out.read((char *)&a, sizeof(double));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const long a)
{
	in.write((const char *)&a, sizeof(long));
	return in;
}

static std::iostream & operator>>(std::iostream & out, long &a)
{
	out.read((char *)&a, sizeof(long));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const unsigned long a)
{
	in.write((const char *)&a, sizeof(unsigned long));
	return in;
}

static std::iostream & operator>>(std::iostream & out, unsigned long &a)
{
	out.read((char *)&a, sizeof(unsigned long));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const long long a)
{
	in.write((const char *)&a, sizeof(long long));
	return in;
}

static std::iostream & operator>>(std::iostream & out, long long &a)
{
	out.read((char *)&a, sizeof(long long));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const unsigned long long a)
{
	in.write((const char *)&a, sizeof(unsigned long long));
	return in;
}

static std::iostream & operator>>(std::iostream & out, unsigned long long &a)
{
	out.read((char *)&a, sizeof(unsigned long long));
	return out;
}

static std::iostream & operator<<(std::iostream & in, const char * a)
{
	int size = strlen(a) + 1;
	in << size;
	in.write(a, size);
	return in;
}

static std::iostream & operator>>(std::iostream & out, char * &a)
{
	int size = 0;
	out >> size;
	a = (char *)malloc(size * sizeof(char));
	out.read(a, size);
	return out;
}

static std::iostream & operator<<(std::iostream & in, const std::string &a)
{
	in << a.c_str();
	return in;
}

static std::iostream & operator>>(std::iostream & out, std::string &a)
{
	char *buf = 0;
	out >> buf;
	a = buf;
	free(buf);
	return out;
}

template<class T>
static std::iostream & operator<<(std::iostream & in, const std::vector<T> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d;
	}

	return in;
}

template<class T>
static std::iostream & operator>>(std::iostream & out, std::vector<T> &a)
{
	int size = 0;
	out >> size;
	a.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		a.emplace_back();
		out >> a.back();
	}

	return out;
}

template<class T>
static std::iostream & operator<<(std::iostream & in, const std::list<T> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d;
	}

	return in;
}

template<class T>
static std::iostream & operator>>(std::iostream & out, std::list<T> &a)
{
	int size = 0;
	out >> size;
	a.reverse(size);
	for (int i = 0; i < size; ++i)
	{
		a.emplace_back();
		out >> a.back();
	}

	return out;
}

template<class T>
static std::iostream & operator<<(std::iostream & in, const std::deque<T> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d;
	}

	return in;
}

template<class T>
static std::iostream & operator>>(std::iostream & out, std::deque<T> &a)
{
	int size = 0;
	out >> size;

	for (int i = 0; i < size; ++i)
	{
		a.emplace_back();
		out >> a.back();
	}

	return out;
}


template<class T>
static std::iostream & operator<<(std::iostream & in, const std::set<T> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d;
	}

	return in;
}

template<class T>
static std::iostream & operator>>(std::iostream & out, std::set<T> &a)
{
	int size = 0;
	out >> size;
	for (int i = 0; i < size; ++i)
	{
		T tmp;
		out >> tmp;
		a.insert(tmp);
	}

	return out;
}

template<class Key, class Value>
static std::iostream & operator<<(std::iostream & in, const std::map<Key, Value> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d.first << d.second;
	}

	return in;
}

template<class Key, class Value>
static std::iostream & operator>>(std::iostream & out, std::map<Key, Value> &a)
{
	int size = 0;
	out >> size;
	for (int i = 0; i < size; ++i)
	{
		Key k;
		out >> k;
		out >> a[k];
	}

	return out;
}

template<class Key, class Value>
static std::iostream & operator<<(std::iostream & in, const std::unordered_map<Key, Value> &a)
{
	in << a.size();
	for each (auto & d in a)
	{
		in << d.first << d.second;
	}

	return in;
}

template<class Key, class Value>
static std::iostream & operator>>(std::iostream & out, std::unordered_map<Key, Value> &a)
{
	int size = 0;
	out >> size;
	a.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		Key k;
		out >> k;
		out >> a[k];
	}

	return out;
}
