/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alion <alion@student.s19.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 10:03:44 by clegros           #+#    #+#             */
/*   Updated: 2024/09/25 14:25:04 by alion            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/cub3D.h"

int	ft_exit_window(t_env *e)
{
	if (e->img)
		mlx_destroy_image(e->mlx, e->img);
	if (e->win)
		mlx_destroy_window(e->mlx, e->win);
	exit(1);
}

static void	init(t_env *e)
{
	e->keys[KEY_W] = 0;
	e->keys[KEY_A] = 0;
	e->keys[KEY_S] = 0;
	e->keys[KEY_D] = 0;
	e->keys[KEY_LEFT] = 0;
	e->keys[KEY_RIGHT] = 0;
	e->pos_x = 0;
	e->pos_y = 0;
	e->dir_x = 0;
	e->dir_y = 0;
	e->plane_x = 0;
	e->plane_y = 0;
	e->map_height = MAP_HEIGHT;
	e->map_width = MAP_WIDTH;
	e->map.map_height = 0;
	e->map.map_width = NULL;
	e->map.world_map = NULL;
}

int	size_fd(char **argv)
{
	int		i;
	int		fd;
	char	*line;

	i = 1;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		ft_printf("Error\nNo fd\n");
		exit(1);
	}
	line = get_next_line(fd);
	while (line != NULL)
	{
		i++;
		if (line == NULL)
			break ;
		line = get_next_line(fd);
	}
	close(fd);
	return (i);
}

void	cleanup(t_map map)
{
	int	i;

	i = 0;
	while (i < map.map_height)
	{
		if (map.map[i])
			free(map.map[i]);
		i++;
	}
	free(map.map);
	exit(0);
}

int	main(int argc, char **argv)
{
	t_env	e;
	t_map	map;
	int		i;

	e.mlx = mlx_init();
	if (!e.mlx)
		ft_printf("Error\nNot correct mlx\n");
	i = size_fd(argv);
	map.map = ft_calloc((i), sizeof(char *));
	if (!map.map)
		ft_exit(map, "Error\nNot correct malloc\n", 0);
	parsing(argc, argv, &map, &e);
	e.win = mlx_new_window(e.mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "cub3D");
	e.img = mlx_new_image(e.mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	e.data = (int *)mlx_get_data_addr(e.img, &e.bpp, &e.sizeline, &e.endian);
	init(&e);
	load_map(&e, argv[1]);
	mlx_hook(e.win, 2, 1L << 0, key_press, &e);
	mlx_hook(e.win, 3, 1L << 1, key_release, &e);
	mlx_loop_hook(e.mlx, render_scene, &e);
	mlx_hook(e.win, 17, 1L << 2, ft_exit_window, &e);
	mlx_loop(e.mlx);
	cleanup(map);
	return (0);
}

	//system("leaks cub3D");
	//mlx_hook(e.win, 6, 0, mouse_hook, &e);
	//mlx_mouse_hide(e.mlx, e.win