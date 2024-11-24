/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alion <alion@student.s19.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:36:17 by clegros           #+#    #+#             */
/*   Updated: 2024/09/24 12:52:08 by alion            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/cub3D.h"

void	load_map(t_env *e, const char *filename)
{
	int		fd;
	char	*line;
	char	*tmp_line;
	int		row;

	fd = skip_and_open_file(filename, e->i);
	if (fd == -1)
		return ;
	row = 0;
	while (1)
	{
		tmp_line = get_next_line(fd);
		if (tmp_line == NULL)
			break ;
		line = tmp_line;
		resize_map(e);
		parse_line(e, line, row);
		e->map.map_height++;
		free(line);
		row++;
	}
	close(fd);
}

void	display_map(int **map, int map_height, int *map_width)
{
	int	i;
	int	j;

	i = 0;
	while (i < map_height)
	{
		j = 0;
		while (j < map_width[i])
		{
			ft_putnbr_fd(map[i][j], 1);
			j++;
		}
		ft_putchar_fd('\n', 1);
		i++;
	}
