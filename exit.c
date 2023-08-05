/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heddahbi <heddahbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 15:24:39 by heddahbi          #+#    #+#             */
/*   Updated: 2023/08/05 15:27:12 by heddahbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_exit_error(char *str, int a)
{
	ft_putstr_fd("exit\n", 1);
	ft_error("minishell: exit: ", str, ": numeric argument required\n");
	exit_st.status = a;
	exit(a);
}

void	handle_digit_exit(t_command *cmd)
{
	int	a;

	a = ft_atoi(cmd->args[1]);
	if (cmd->args[2] != NULL)
	{
		ft_putstr_fd("exit\n", 1);
		exit_st.status = 1;
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		exit (1);
	}
	else
	{
		if (a == 255)
			print_exit_error(cmd->args[1], a);
		else
		{
			ft_putstr_fd("exit\n", 1);
			exit_st.status = a;
			exit(a);
		}
	}
}

void	ft_exit(t_command *cmd)
{
	if (cmd->args[1] == NULL)
		exit (0);
	else if (cmd->args[1] != NULL)
	{
		if (ft_isdigit(cmd->args[1][0]) == 0 
			&& (cmd->args[1][0] != '-' && cmd->args[1][0] != '+'))
		{
			ft_putstr_fd("exit\n", 1);
			ft_error("minishell: exit: ", cmd->args[1],
				": numeric argument required\n");
			exit_st.status = 255;
			exit(255);
		}
		else if (ft_isdigit(cmd->args[1][0]) == 1 
			|| (cmd->args[1][0] == '-' || cmd->args[1][0] == '+'))
			handle_digit_exit(cmd);
	}
}
