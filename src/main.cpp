/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 01:52:59 by nathan            #+#    #+#             */
/*   Updated: 2021/10/19 16:03:18 by nathan           ###   ########.fr       */
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

int		main( int argc, char *argv[] )
{
	if (!appWindow::init())
		return (0);

	if (glewInit() != GLEW_OK)
	{
		std::cerr <<  "Failed to initialize GLEW\n" << std::endl;
		return 0;
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);// renders only visible squares of cubes
	glClearColor(0.2, 0.2, 0.2, 1.0f);

	unsigned long	seed = DEFAULT_SEED;
	if (argc > 1)
	{
		seed = stoi(argv[1]);
	}


	Camera camera;
	World* world = new World();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Chunk* chnk = new Chunk(i * CHUNK_WIDTH, j * CHUNK_DEPTH);
			world->addObject(chnk);
		}
	}
	//RectangularCuboid* floor = new RectangularCuboid(1000, 0.1, 1000);
	//floor->setID("floor");
	//floor->setColor({0.6, 0.7, 0.6});
	//floor->setShader(new Shader("floor.vert", "floor.frag"));

	world->setCamera(camera);
	HeightMap heightMap = VoxelGenerator::createMap(seed);
	//world->addObject(floor);
	Loop::setWorld(world);

	Loop::loop();
	delete world;

	return 1;
}
