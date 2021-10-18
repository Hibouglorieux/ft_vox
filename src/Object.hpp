/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 19:05:09 by nathan            #+#    #+#             */
/*   Updated: 2021/10/18 15:52:02 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef OBJECT_CLASS_H
# define OBJECT_CLASS_H

#include "Matrix.hpp"
#include "Shader.hpp"
#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 600.0f //TODO change that to have the real window size

class Object {
public:
	Object(void);
	virtual void draw(Matrix* viewMat) = 0;
	virtual ~Object(void);
	virtual Vec3 getPos() const = 0;
	static void setProjMat(Matrix projMat);
protected:
	static Matrix projMat;
};

#endif
