/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:32 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 21:49:37 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CAMERA_CLASS_H
# define CAMERA_CLASS_H
#include "Matrix.hpp"
#include "appWindow.hpp"
#include "Vec3.hpp"
# include "Chunk.hpp"

#define Y_MOVE_SPEED 1.0f

class Camera {
public:
	Camera(void);
	Camera(float x, float y, float z);
	Camera(Vec3 translation);
	void reset();
	~Camera(void) {};
	void move(bool forward, bool backward, bool right, bool left, float speedFactor);
	void moveUp(float distance = Y_MOVE_SPEED);
	void moveDown(float distance = Y_MOVE_SPEED);
	void rotate(double x, double y);
	Vec3 getPos() const;// TODO maybe test with a matrix.getTranslationVector instead for lookAt
	Matrix getMatrix();
	std::pair<Vec3, Vec3> unProject(float mouseX, float mouseY, Matrix projMat);
	Vec3 unProjectToOrigin(float mouseX, float mouseY, Matrix projMat);

	void setChunkUpdate(bool value) { shouldUpdateChunks = value; };
	bool getChunkUpdate() const { return shouldUpdateChunks; };
	Vec3 getChunkPos() const { return currentChunk; };
private:
	Vec3 getDirection() const;// meant to convert degree to rad
	void actualizeView();
	Matrix view;
	Vec3 dir;
	Vec3 pos;

	Vec3 currentChunk;
	bool shouldUpdateChunks;
};

#endif
