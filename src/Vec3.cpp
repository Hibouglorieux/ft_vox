/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec3.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/26 16:12:31 by nathan            #+#    #+#             */
/*   Updated: 2020/12/23 21:46:00 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Vec3.hpp"
#include <string>
#include "Utilities.h"

Vec3::Vec3( void )
{
	x = 0;
	y = 0;
	z = 0;
}

Vec3::Vec3( float xx, float yy, float zz)
{
	x = xx;
	y = yy;
	z = zz;
}

Vec3::Vec3( const Vec3& copy)
{
	x = copy.x;
	y = copy.y;
	z = copy.z;
}

float Vec3::getLength() const
{
	return sqrtf( x * x + y * y + z * z );
}

Vec3 Vec3::getAngle() const
{
	Vec3 angle;

	angle.x = TO_DEGREE(atan2(sqrtf( y * y + z * z ), x));
	angle.y = TO_DEGREE(atan2(sqrtf( z * z + x * x ), y));
	angle.z = TO_DEGREE(atan2(sqrtf( x * x + y * y ), z));
	return angle;
}

Vec3 Vec3::getNormalized() const 
{
	float length = this->getLength();	

	if (length == 0)
		return Vec3::ZERO;
	return Vec3(x / length, y / length, z / length);
}

float Vec3::dot(Vec3 rhs) const
{
	return (x * rhs.x + y * rhs.y + z * rhs.z);
}

std::string Vec3::toString() const
{
	std::stringstream ss;
	ss.precision(2); // print only 2 first decimals
	ss << std::fixed; // put precision for 0.00 too
	ss << "{";
	ss.width(4);
	ss << " " << x;
	ss << " " << y;
	ss << " " << z;
	ss << " }";
	ss << std::endl;
	return ss.str();
}

void Vec3::print() const
{
	std::cout << toString();
}

Vec3 Vec3::operator+(const Vec3& rhs) const
{
	Vec3 newVector( x + rhs.x, y + rhs.y, z + rhs.z );
	return newVector;
}

Vec3 Vec3::operator+=( const Vec3& rhs)
{
	*this = *this + rhs;
	return *this;
}

Vec3 Vec3::operator-=( const Vec3& rhs)
{
	*this = *this - rhs;
	return *this;
}

Vec3 Vec3::operator-(const Vec3& rhs) const
{
	Vec3 newVector( x - rhs.x, y - rhs.y, z - rhs.z );
	return newVector;
}

Vec3 Vec3::operator*(const float scale) const
{
	Vec3 newVector( x * scale, y * scale, z * scale );
	return newVector;
}

Vec3 Vec3::operator*(const Vec3& rhs) const
{
	Vec3 newVector( x * rhs.x, y * rhs.y, z * rhs.z);
	return newVector;
}

bool Vec3::operator==(const Vec3& rhs) const
{
	if (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z)
		return true;
	return false;
}

bool Vec3::operator!=(const Vec3& rhs) const
{
	return !(*this == rhs);
}

float Vec3::at(const int i) const
{
	if (i == 0) return x;
	if (i == 1) return y;
	if (i == 2) return z;
	std::string err("Trying to access " + std::to_string(i) + " on Vec3 object");
	std::cerr << err << std::endl;
	throw err;
}

float& Vec3::operator[]( const int i )
{
	if (i == 0) return x;
	if (i == 1) return y;
	if (i == 2) return z;
	std::string err("Trying to access " + std::to_string(i) + " on Vec3 object");
	std::cerr << err << std::endl;
	throw err;
}

Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

const Vec3 Vec3::ZERO(0, 0 ,0);
const Vec3 Vec3::ONE(1, 1 ,1);
