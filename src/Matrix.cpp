/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Matrix.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/25 11:07:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 12:52:35 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Matrix.hpp"
#include "Utilities.h"

/*
constexpr std::vector<std::vector<float>> Matrix::Projection = {
	float tanHalfFov = tan(to_rad(fov) / 2); = {
		{1 / (aspect * tanHalfFov), 0 , 0, 0},
		{0, 1 / tanHalfFov, 0, 0},
		{0, 0 , -(far + near) / (far - near), (-2 * far * near) / (far - near)},
		{0, 0 , -1, 1}}
};
*/

Matrix::Matrix( void )
{
	data = {
		{1, 0, 0, 0,},
		{0, 1, 0, 0,},
		{0, 0, 1, 0,},
		{0, 0, 0, 1}
	};
}

Matrix::Matrix( std::vector<std::vector<float>> dataParam )
{
	size_t i = 0;
	if (dataParam.size() != 4)
	{
		std::cerr << "Error: wrong size of matrix given as constructor (it is not 4x4)\n" + toString(dataParam) << std::endl;
		abort();
	}
	while ( i < dataParam.size() )
	{
		if (dataParam[i].size() != 4)
		{
			std::cerr << "Error: wrong size of matrix given as constructor (it is not 4x4)\n" + toString(dataParam) << std::endl;
			abort();
		}
		i++;
	}
	data = dataParam;
}

Matrix::Matrix( const Matrix& copy )
{
	data = copy.data;
}

Matrix Matrix::createProjMatrix( float fov, float aspect, float near, float far )
{
	float tanHalfFov = tan( TO_RAD(fov) / 2 );
	Matrix projMatrix({
		{1 / ( aspect * tanHalfFov ), 0 , 0, 0},
		{0, 1 / tanHalfFov, 0, 0},
		{0, 0 , -( far + near ) / ( far - near ), ( -2 * far * near ) / ( far - near )},
		{0, 0 , -1, 0}});
	return projMatrix;
}

Matrix Matrix::createTranslationMatrix( float x, float y, float z )
{
	Matrix translationMatrix({
			{1, 0, 0, x},
			{0, 1, 0, y},
			{0, 0, 1, z},
			{0, 0, 0, 1}});
	return translationMatrix;
}

Matrix Matrix::createScaleMatrix( float x, float y, float z )
{
	Matrix scaleMatrix({
			{x, 0, 0, 0},
			{0, y, 0, 0},
			{0, 0, z, 0},
			{0, 0, 0, 1}});
	return scaleMatrix;
}

Matrix Matrix::createRotationMatrix( RotationDirection dir, float angle )
{
	Matrix rotationMatrix;
	angle = TO_RAD(angle);
	if ( dir == RotationDirection::X )
	{
		rotationMatrix = Matrix({
			{1,     0,            0,       0},
			{0, cosf(angle), -sinf(angle), 0},
			{0, sinf(angle),  cosf(angle), 0},
			{0,     0,            0,       1}
		});
	}
	if ( dir == RotationDirection::Y )
	{
		rotationMatrix = Matrix({
			{ cosf(angle), 0, sinf(angle), 0},
			{    0,        1,      0,      0},
			{-sinf(angle), 0, cosf(angle), 0},
			{    0,        0,      0,      1}
		});
	}
	if ( dir == RotationDirection::Z )
	{
		rotationMatrix = Matrix({
			{cosf(angle), -sinf(angle), 0, 0},
			{sinf(angle),  cosf(angle), 0, 0},
			{    0,            0,       1, 0},
			{    0,            0,       0, 1}
		});
	}
	return rotationMatrix;
}


std::string Matrix::toString( std::vector<std::vector<float>> dataParam ) const
{
	std::stringstream ss;
	ss.precision(2); // print only 2 first decimals
	ss << std::fixed; // put precision for 0.00 too
	for ( size_t i = 0; i < dataParam.size(); i++ )
	{
		ss << "{";
		for ( size_t j = 0; j < dataParam[i].size(); j++ )
		{
			ss.width(4);
			ss << " " << dataParam[i][j];
		}
		ss << " }";
		ss << std::endl;
	}
	return ss.str();
}

void Matrix::print() const
{
	std::cout << toString( data ) << std::endl;
}

Vec3 Matrix::vectorMult(const Vec3 vec) const
{
	Vec3 newVec;
	float w = 0.0f;
	for (int i = 0; i < 3; 	i++)	
		for (int j = 0; j < 4; j++)
		{
			newVec[i] += this->data[i][j] * (j > 2 ? 1 : vec.at(j));
		}

	for (int j = 0; j < 4; j++)
		w += this->data[3][j] * (j > 2 ? 1 : vec.at(j));
	return (newVec * (1 / w));
}

Matrix::~Matrix()
{
}

GLfloat*	Matrix::exportForGL()
{
	exportData.resize(16);
	int i;

	i = 0;
	while ( i < 4 )
	{
		exportData[i * 4 + 0] = (GLfloat)data[i][0];
		exportData[i * 4 + 1] = (GLfloat)data[i][1];
		exportData[i * 4 + 2] = (GLfloat)data[i][2];
		exportData[i * 4 + 3] = (GLfloat)data[i][3];
		i++;
	}
	return exportData.data();
}

Matrix Matrix::operator*( const Matrix& rhs ) const
{
	int firstRow = 0;
	int firstColumn = 0;
	int secondColumn = 0;

	Matrix newMatrix( { {0, 0, 0, 0},
						{0, 0, 0, 0},
						{0, 0, 0, 0},
						{0, 0, 0, 0} });
	firstRow = 0;
	while ( firstRow < 4 )
	{
		firstColumn = 0;
		while ( firstColumn < 4 )
		{
			secondColumn = 0;
			while ( secondColumn < 4 )
			{
				newMatrix.data[firstRow][firstColumn] += this->data[firstRow][secondColumn] * rhs.data[secondColumn][firstColumn];
				secondColumn++;
			}
			firstColumn++;
		}
		firstRow++;
	}
	return newMatrix;
}

Matrix& Matrix::operator=( const Matrix& rhs )
{
	this->data = rhs.data;
	return *this;
}

Vec3 Matrix::operator*( const Vec3& rhs ) const
{
	return this->vectorMult(rhs);
}

Matrix Matrix::operator*=( const Matrix& rhs )
{
	*this = *this * rhs;
	return *this;
}	

std::vector<float>& Matrix::operator[]( const int i )
{
	return this->data[i];
}

Matrix Matrix::invert()
{
	Matrix inv;
	Matrix ret;
	Matrix& m = *this;
	float det;

	inv[0][0] =   m[1][1]*m[2][2]*m[3][3] - m[1][1]*m[2][3]*m[3][2] - m[2][1]*m[1][2]*m[3][3]
		+ m[2][1]*m[1][3]*m[3][2] + m[3][1]*m[1][2]*m[2][3] - m[3][1]*m[1][3]*m[2][2];

	inv[1][0] =  -m[1][0]*m[2][2]*m[3][3] + m[1][0]*m[2][3]*m[3][2] + m[2][0]*m[1][2]*m[3][3]
		- m[2][0]*m[1][3]*m[3][2] - m[3][0]*m[1][2]*m[2][3] + m[3][0]*m[1][3]*m[2][2];

	inv[2][0] =   m[1][0]*m[2][1]*m[3][3] - m[1][0]*m[2][3]*m[3][1] - m[2][0]*m[1][1]*m[3][3]
		+ m[2][0]*m[1][3]*m[3][1] + m[3][0]*m[1][1]*m[2][3] - m[3][0]*m[1][3]*m[2][1];

	inv[3][0] = -m[1][0]*m[2][1]*m[3][2] + m[1][0]*m[2][2]*m[3][1] + m[2][0]*m[1][1]*m[3][2]
		- m[2][0]*m[1][2]*m[3][1] - m[3][0]*m[1][1]*m[2][2] + m[3][0]*m[1][2]*m[2][1];

	inv[0][1] =  -m[0][1]*m[2][2]*m[3][3] + m[0][1]*m[2][3]*m[3][2] + m[2][1]*m[0][2]*m[3][3]
		- m[2][1]*m[0][3]*m[3][2] - m[3][1]*m[0][2]*m[2][3] + m[3][1]*m[0][3]*m[2][2];

	inv[1][1] =   m[0][0]*m[2][2]*m[3][3] - m[0][0]*m[2][3]*m[3][2] - m[2][0]*m[0][2]*m[3][3]
		+ m[2][0]*m[0][3]*m[3][2] + m[3][0]*m[0][2]*m[2][3] - m[3][0]*m[0][3]*m[2][2];

	inv[2][1] =  -m[0][0]*m[2][1]*m[3][3] + m[0][0]*m[2][3]*m[3][1] + m[2][0]*m[0][1]*m[3][3]
		- m[2][0]*m[0][3]*m[3][1] - m[3][0]*m[0][1]*m[2][3] + m[3][0]*m[0][3]*m[2][1];

	inv[3][1] =  m[0][0]*m[2][1]*m[3][2] - m[0][0]*m[2][2]*m[3][1] - m[2][0]*m[0][1]*m[3][2]
		+ m[2][0]*m[0][2]*m[3][1] + m[3][0]*m[0][1]*m[2][2] - m[3][0]*m[0][2]*m[2][1];

	inv[0][2] =   m[0][1]*m[1][2]*m[3][3] - m[0][1]*m[1][3]*m[3][2] - m[1][1]*m[0][2]*m[3][3]
		+ m[1][1]*m[0][3]*m[3][2] + m[3][1]*m[0][2]*m[1][3] - m[3][1]*m[0][3]*m[1][2];

	inv[1][2] =  -m[0][0]*m[1][2]*m[3][3] + m[0][0]*m[1][3]*m[3][2] + m[1][0]*m[0][2]*m[3][3]
		- m[1][0]*m[0][3]*m[3][2] - m[3][0]*m[0][2]*m[1][3] + m[3][0]*m[0][3]*m[1][2];

	inv[2][2] =  m[0][0]*m[1][1]*m[3][3] - m[0][0]*m[1][3]*m[3][1] - m[1][0]*m[0][1]*m[3][3]
		+ m[1][0]*m[0][3]*m[3][1] + m[3][0]*m[0][1]*m[1][3] - m[3][0]*m[0][3]*m[1][1];

	inv[3][2] = -m[0][0]*m[1][1]*m[3][2] + m[0][0]*m[1][2]*m[3][1] + m[1][0]*m[0][1]*m[3][2]
		- m[1][0]*m[0][2]*m[3][1] - m[3][0]*m[0][1]*m[1][2] + m[3][0]*m[0][2]*m[1][1];

	inv[0][3] =  -m[0][1]*m[1][2]*m[2][3] + m[0][1]*m[1][3]*m[2][2] + m[1][1]*m[0][2]*m[2][3]
		- m[1][1]*m[0][3]*m[2][2] - m[2][1]*m[0][2]*m[1][3] + m[2][1]*m[0][3]*m[1][2];

	inv[1][3] =   m[0][0]*m[1][2]*m[2][3] - m[0][0]*m[1][3]*m[2][2] - m[1][0]*m[0][2]*m[2][3]
		+ m[1][0]*m[0][3]*m[2][2] + m[2][0]*m[0][2]*m[1][3] - m[2][0]*m[0][3]*m[1][2];

	inv[2][3] = -m[0][0]*m[1][1]*m[2][3] + m[0][0]*m[1][3]*m[2][1] + m[1][0]*m[0][1]*m[2][3]
		- m[1][0]*m[0][3]*m[2][1] - m[2][0]*m[0][1]*m[1][3] + m[2][0]*m[0][3]*m[1][1];

	inv[3][3] =  m[0][0]*m[1][1]*m[2][2] - m[0][0]*m[1][2]*m[2][1] - m[1][0]*m[0][1]*m[2][2]
		+ m[1][0]*m[0][2]*m[2][1] + m[2][0]*m[0][1]*m[1][2] - m[2][0]*m[0][2]*m[1][1];

	det = m[0][0]*inv[0][0] + m[0][1]*inv[1][0] + m[0][2]*inv[2][0] + m[0][3]*inv[3][0];
	if (det == 0)
	{
		std::cout << "Error with invert matrix, division by zero not possible" << std::endl;
		return *this;//TODO this cannot happen, right ? :)
	}

	det = 1.0f / det;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		ret[i][j] = inv[i][j] * det;
	return ret;
}

Matrix Matrix::transpose() const
{
	Matrix transposed;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transposed.data[j][i] = data[i][j];
		}
	}
	return transposed;
}
/*
Matix Matrix::scaleTranslationMatrix( Matrix transMat, const Matrix& scaleMat)
{
	transMat.data[3][0] * = scaleMat.data[0][0];
	return (transMat);
}
*/


