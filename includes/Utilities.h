/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utilities.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 02:07:43 by nathan            #+#    #+#             */
/*   Updated: 2022/02/27 09:57:04 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef UTILITIES_H
# define UTILITIES_H

#include <cmath>

#  define TO_RAD(x) (x / 180.0 * M_PI)
#  define TO_DEGREE(x) (x * 180.0 / M_PI)

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ " : " S2(__LINE__)

#endif
