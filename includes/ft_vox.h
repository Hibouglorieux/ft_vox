/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vox.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 13:12:12 by nathan            #+#    #+#             */
/*   Updated: 2021/10/20 13:21:37 by nathan           ###   ########.fr       */
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

#define BLOC_WIDTH_PER_CHUNK 32
#define MAX_NB_OF_CHUNK (WIDTH / BLOC_WIDTH_PER_CHUNK)

#endif
