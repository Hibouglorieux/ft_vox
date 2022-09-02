/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2022/09/02 18:28:40 by nallani          ###   ########.fr       */
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
#include "TextManager.hpp"
#include "Faces.hpp"

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
	TextManager::init();

	// openGL parameters
	glClearColor(0.2, 0.2, 0.2, 1.0f);

	return (1);
}

std::vector<Face*> getFaces() // initializes faces data
{
	std::vector<Face*> faces;
	faces.push_back(new FrontFace());
	faces.push_back(new BackFace());
	faces.push_back(new BottomFace());
	faces.push_back(new TopFace());
	faces.push_back(new LeftFace());
	faces.push_back(new RightFace());

	return faces;
}

int		main( int argc, char *argv[] )
{
	unsigned long	seed = DEFAULT_SEED; //std::srand(std::time(nullptr));
	if (argc > 1)
	{
		try
		{
			seed = stoi(argv[1]);
		}
		catch(...)
		{
			std::cout << "usage: ft_vox [seed]" << std::endl;
			return (0);
		}
	}

	if (!initialize())
		return (0);

	ResourceManager::loadPack();

	VoxelGenerator::initialize(seed);
	VoxelGenerator::initialize(seed, true);
	std::vector<Face*> initFaces = getFaces();
	World* world = new World();

	Loop::setWorld(world);

	Loop::loop();
	std::cout << glGetError() << std::endl;
	delete world;
	TextManager::clear();
	VoxelGenerator::clear();
	RectangularCuboid::clear();
	ResourceManager::deletePack();

	for (Face* face : initFaces)
	{
		delete face;
	}

	glfwTerminate();
	return 1;
}
