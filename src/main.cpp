/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2022/02/26 01:46:30 by nathan           ###   ########.fr       */
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
	World* world = new World();

	Loop::setWorld(world);

	Loop::loop();
	std::cout << glGetError() << std::endl;
	delete world;
	VoxelGenerator::clear();
	RectangularCuboid::clear();

	return 1;
}
