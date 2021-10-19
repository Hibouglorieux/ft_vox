/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/08 19:05:09 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 15:59:01 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef OBJECT_CLASS_H
# define OBJECT_CLASS_H

#include "Matrix.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 600.0f //TODO change that to have the real window size

class Object {
public:
	Object(void);
	virtual void	draw(Shader* shader) = 0;
	virtual ~Object(void);
	static void setProjMat(Matrix projMat);
	static Matrix getProjMat(){return projMat;} // TODO might need to remove
protected:
	static Matrix projMat;
};

#endif
