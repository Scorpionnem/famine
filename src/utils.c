/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 14:43:37 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/18 14:43:56 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char	*strjoin(char const *s1, char const *s2)
{
	char	*dest;
	size_t	len;

	if (!s1 || !s2)
		return (0);
	len = (strlen(s1) + strlen(s2) + 1);
	dest = malloc(len * sizeof(char));
	if (dest == NULL)
		return (NULL);
	strncpy(dest, s1, len);
	strncat(dest, s2, len);
	return ((char *)dest);
}
