/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Matrix.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/25 11:07:43 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 16:15:36 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MATRIX_CLASS_H
# define MATRIX_CLASS_H
#include <vector>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Vec3.hpp"

class Matrix {
public:
	enum RotationDirection {
		X,
		Y,
		Z
	};
	Matrix( void );// also is identity matrix
	Matrix( std::vector<std::vector<float>> dataParam );
	Matrix( const Matrix& copy );

	static Matrix createProjMatrix( float fov, float aspect, float near, float far );// takes fov as degree
	static Matrix createTranslationMatrix( float x, float y, float z );
	static Matrix createTranslationMatrix(Vec3 vec) {return createTranslationMatrix(vec.x, vec.y, vec.z);}
	static Matrix createScaleMatrix( float x, float y, float z );
	static Matrix createScaleMatrix(Vec3 vec) {return createScaleMatrix(vec.x, vec.y, vec.z);}
	static Matrix createRotationMatrix( RotationDirection dir, float angle ); //takes angle as degree

	Vec3 vectorMult(const Vec3 vec) const;
	~Matrix( void );
	Matrix invert(  );
	GLfloat* exportForGL(  );
	Matrix transpose(  ) const;
	Matrix operator*( const Matrix& rhs ) const;
	Vec3 operator*( const Vec3& rhs) const;
	Matrix operator*=( const Matrix& rhs );
	std::vector<float>& operator[]( const int i );
	void print() const;
	std::string toString( std::vector<std::vector<float>> dataParam ) const;

private:
	std::vector<std::vector<float>> data;
	std::vector<GLfloat> exportData;
};

#endif
