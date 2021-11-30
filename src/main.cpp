/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2021/11/30 18:06:29 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <GL/glew.h>
#include "RectangularCuboid.hpp"
#include "appWindow.hpp"
#include "Loop.hpp"
#include "World.hpp"
#include "Matrix.hpp"
#include "Chunk.hpp"
#include "VoxelGenerator.hpp"

#define DEFAULT_SEED 42

using namespace std;

int	initialize()
{
	if (!appWindow::init())
		return (0);
	if (glewInit() != GLEW_OK)
	{
		std::cerr <<  "Failed to initialize GLEW\n" << std::endl;
		return (0);
	}
	RectangularCuboid::initialize();

	// openGL parameters
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2, 0.2, 0.2, 1.0f);
	//glCullFace(GL_BACK);// renders only visible squares of cubes

	return (1);
}

int		main( int argc, char *argv[] )
{
	if (!initialize())
		return (0);

	unsigned long	seed = DEFAULT_SEED; //std::srand(std::time(nullptr));
	if (argc > 1)
	{
		seed = stoi(argv[1]);
	}

	ResourceManager::loadPack();

	VoxelGenerator::initialize(seed);
	VoxelGenerator::initialize(seed, true);
	World* world = new World();

	Loop::setWorld(world);
	
	Loop::loop();
	delete world;
	VoxelGenerator::clear();
	RectangularCuboid::clear();

	return 1;
}
