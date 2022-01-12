/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vox.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:12:12 by nathan            #+#    #+#             */
/*   Updated: 2021/11/30 17:07:44 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FT_VOX_CLASS_H
# define FT_VOX_CLASS_H

#define EMPTY 0
#define HAS 1

#define HEIGHT 256
#define WIDTH  16384
#define LENGTH WIDTH

#define NEAR 0.1f
#define PROJECT_NAME "ft_vox"
#define FAR 1500.0f
#define FOV 80.0f
#define FOV_RAD FOV * M_PI / 180.0

#define SEC_TO_MICROSEC 1000000
#define BLOC_WIDTH_PER_CHUNK 16
//#define MAX_NB_OF_CHUNK (WIDTH / BLOC_WIDTH_PER_CHUNK)
#define MAX_NB_OF_CHUNK 1024 // TODO this has to be (WIDTH / BLOCK_WIDTH_PER_CHUNK) but chang be changed to have a better view of the Heightmap on less chunks

/* Profiler code:
 
#include <chrono>
	auto start = std::chrono::high_resolution_clock::now();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << duration.count() << std::endl;
*/

#endif
