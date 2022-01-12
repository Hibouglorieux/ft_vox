/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec2.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:33:53 by nathan            #+#    #+#             */
/*   Updated: 2021/12/02 18:25:28 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Vec2.hpp"

Vec2::Vec2( void )
{
	x = 0;
	y = 0;
}

Vec2::Vec2(int xx, int yy)
{
	x = xx;
	y = yy;
}

Vec2::Vec2(const Vec2& copy)
{
	x = copy.x;
	y = copy.y;
}

float Vec2::getLength() const
{
	return sqrtf( x * x + y * y);
}

std::string Vec2::toString() const
{
	std::stringstream ss;
	ss << std::fixed; // put precision for 0.00 too
	ss << "{";
	ss << " " << x;
	ss << " " << y;
	ss << " }";
	ss << std::endl;
	return ss.str();
}

void Vec2::print() const
{
	std::cout << toString();
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	Vec2 newVector( x + rhs.x, y + rhs.y);
	return newVector;
}

Vec2 Vec2::operator+=( const Vec2& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vec2 Vec2::operator-=( const Vec2& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	Vec2 newVector( x - rhs.x, y - rhs.y);
	return newVector;
}

Vec2 Vec2::operator*(const float scale) const
{
	Vec2 newVector( x * scale, y * scale);
	return newVector;
}

Vec2 Vec2::operator*(const Vec2& rhs) const
{
	Vec2 newVector( x * rhs.x, y * rhs.y);
	return newVector;
}

bool Vec2::operator==(const Vec2& rhs) const
{
	if (this->x == rhs.x && this->y == rhs.y)
		return true;
	return false;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return !(*this == rhs);
}

int Vec2::at(const int i) const
{
	if (i == 0) return x;
	if (i == 1) return y;
	std::string err("Trying to access " + std::to_string(i) + " on Vec2 object");
	std::cerr << err << std::endl;
	throw err;
}

int& Vec2::operator[]( const int i )
{
	if (i == 0) return x;
	if (i == 1) return y;
	std::string err("Trying to access " + std::to_string(i) + " on Vec2 object");
	std::cerr << err << std::endl;
	throw err;
}

Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}

Vec2& Vec2::operator=(const Vec2& rhs)
{
	this->x = rhs.x;
	this->y = rhs.y;
	return *this;
}

bool Vec2::operator<(const Vec2& rhs) const// this is only for map
{
	if (y > rhs.y)
		return false;
	if (y == rhs.y && x > rhs.x)
		return false;
	if (*this == rhs)
		return false;
	return true;
}

bool Vec2::isSmaller(const Vec2& rhs) const
{
	return ((abs(x) + abs(y)) < (abs(rhs.x) + abs(rhs.y)));
}

float Vec2::dot(Vec2 rhs) const
{
	return (x * rhs.x + y * rhs.y);
}

bool Vec2::operator>(const Vec2& rhs) const
{
	return !(*this < rhs) && !(*this == rhs);
}
