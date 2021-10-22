/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec2.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/22 12:33:49 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 13:31:24 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VEC2_CLASS_H
# define VEC2_CLASS_H

#include <sstream>
#include <cmath>
#include <string>
#include <iostream>

class Vec2 {
public:
	Vec2(void);
	Vec2(int xx, int yy);
	~Vec2(void){};
	float getLength() const;
	std::string toString() const;
	void print() const;
	Vec2 operator+( const Vec2& rhs ) const;
	Vec2 operator+=( const Vec2& rhs);
	Vec2 operator-( const Vec2& rhs ) const;
	Vec2 operator-=( const Vec2& rhs);
	Vec2 operator*( const float scale ) const;
	Vec2 operator*( const Vec2& rhs ) const;
	bool operator==( const Vec2& rhs ) const;
	bool operator!=( const Vec2& rhs ) const;
	Vec2& operator=( const Vec2& rhs );
	Vec2 operator-() const;
	bool operator<(const Vec2& rhs) const;
	bool operator>(const Vec2& rhs) const;
	int at(const int i) const;
	int& operator[]( const int i );
	int x;
	int y;
private:
};

#endif
