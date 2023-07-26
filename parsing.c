#include "minishell.h"


t_command	*lstlast(t_command *lst)
{
	while (lst && lst->next)
		lst = lst->next;
	return (lst);
}


void my_lstadd_back(t_command **lst, t_command *new)
{
	t_command	*last_add;

	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last_add = lstlast(*lst);
	last_add->next = new;
}


t_command *ft_new(char **args, t_fd fd)
{
	t_command *list;

	list = malloc(sizeof(t_command));
	if (!list)
		return (NULL);
	list->args = args;
	list->fd = fd;
	list->next = NULL;
	return (list);
}

int ft_lstsize(t_lexer *lst)
{
	int i;

	i = 0;
	while (lst != NULL)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}
int calculate_args(t_lexer *tmp)
{
	int i = 0;
	int j = 0;

	while(tmp != NULL && tmp->token != PIPE_LINE)
	{
		if(tmp->token == WORD)
		{
			j = count_strings(tmp->content, " \t\n\v\f\r");
			i += j;
		}
		else if(tmp->token == REDIR_IN || tmp->token == REDIR_OUT || tmp->token == APPEND || tmp->token == HEARDOC)
		{
			tmp = tmp->next;
		}
		tmp = tmp->next;
	}
	return (i);
}


int check_space(char *line)
{
	int i = 0;

	while(line[i])
	{
		if(line[i] == ' ' || line[i] == '\t')
			return (1);
		i++;
	}
	return (0);
}
int is_quote1(char *str)
{
	int i = 0;

	while(str[i])
	{
		if(str[i] == '\"')
			return (1);
		i++;
	}
	return (0);
}

int parse_env(t_env **g_env, char *str)
{
	t_env *env;
	env = *g_env;

	while(env)
	{
		env->value = del_quote(env->value, '\'', '\"');
		if(ft_strcmp(env->value, str) == 0)
			return (1);
		env = env->next;
	}
	return (0);
}

char *add_args(char *str)
{
	str = del_quote(str, '\'', '\"');
	str = ft_strdup(str);
	return (str);
}

char **get_p(char *str)
{
	char **p;
	str = del_quote(str, '\'', '\"');
	p = ft_split1(str, "' ' '\t' '\n' '\v' '\f' '\r'");
	return (p);
}

char **is_word(char *str, char **args, t_env **g_env, int *i)
{
	char **p;
	char *s;
	int k = 0;
	s = ft_expand(str, g_env);
	if (check_space(str) && !is_dquote(str))
	{
		p = get_p(s);
		k = 0;
		while (p[k])
		{
			args[*i] = ft_strdup(p[k]);
			printf("%s\n", args[*i]);
			(*i)++;
			free(p[k++]);
		}
		free(p);
	}
	else
	{
		args[*i] = add_args(str);
		(*i)++;
	}
	return (args);
}
int is_dquote(char *str)
{
	int i = 0;

	while(str[i])
	{
		if(str[i] == '\"')
			return (1);
		i++;
	}
	return (0);
}
int ambiguous_redirect(char *str_next)
{
	if(str_next[0] == '$' && str_next[1] != '\0')
		return (1);
	return (0);
}

int handel_redirout(char *str_next, int fd, t_env **g_env,int a)
{
	char *str;
	(void)a;

	str = ft_strdup(str_next);
	str = ft_expand(str, g_env);
	if(ft_strcmp(str, ""))
	{
		str = del_quote(str, '\'', '\"');
		fd = open(str, O_CREAT | O_WRONLY | O_APPEND, 0644);
		if(fd == -1 )
		{
			write(1, "minishell: ", 11);
			write(1, str, ft_strlen(str));
			perror("");
			return (free(str), -1);
		}
	}
	else
	{
			printf("minishell: %s: ambiguous redirect\n", str_next);
	}
	free(str);
	return (fd);
}
int handel_redirin(char *str_next, int fd, t_env **g_env,int a)
{
	char *str;
	(void)a;

	str = ft_strdup(str_next);
	str = ft_expand(str, g_env);
	if(ft_strcmp(str, ""))
	{
		str = del_quote(str, '\'', '\"');
		fd = open(str, O_RDONLY, 0644);
		if(fd == -1 )
		{
			printf("minishell: %s: No such file or directory\n", str);
			return (free(str), -1);
		}
	}
	else
	{
		printf("minishell: %s: ambiguous redirect\n", str_next);
	}
	free(str);
	return (fd);
}

int parse_redir_out(int type, char *str_next, int fd, t_env **g_env)
{
	int a;
	a = is_dquote(str_next);
	if (type == REDIR_OUT)
	{
		fd  = handel_redirout(str_next, fd, g_env, a);
		if(fd == -1)
			return (-1);
	}
	else if (type == APPEND)
	{
		fd = handel_redirout(str_next, fd, g_env, a);
		if (fd == -1)
			return (-1);
	}
	return fd;
}

int parse_redir_in(int type, char *str_next, int fd, t_env **g_env)
{
	int a;
	a = is_dquote(str_next);
	if (type == REDIR_IN)
	{
		fd = handel_redirin(str_next, fd, g_env, a);
		if(fd == -1)
			return (-1);
	}
	if(type == HEARDOC)
	{
		fd = open("/tmp/srfak", O_RDONLY, 0644);
	}
	return (fd);
}

int  create(char **args, t_command **cmd, t_fd fd , int i)
{
	args[i] = NULL;
	my_lstadd_back(cmd, ft_new(args, fd));
	return (0);
}

char **realloc_args(char **args, int cont)
{
	args = malloc(sizeof(char *) * (cont + 1));
	if(!args)
		return (NULL);
	return (args);
}

t_fd  parse_redirection(int type, char *str_next, t_fd fd, t_env **g_env)
{
	fd.fd_out = parse_redir_out(type, str_next, fd.fd_out, g_env);
	fd.fd_in = parse_redir_in(type, str_next, fd.fd_in, g_env);
	return (fd);
}

t_fd ft_fd(int fd_in, int fd_out)
{
	t_fd fd;

	fd.fd_in = fd_in;
	fd.fd_out = fd_out;
	return (fd);
}

void parsing1(t_lexer *tmp ,char **args, t_env **g_env, t_command **cmd)
{
	int i;
	t_fd fd;

	fd = ft_fd(0, 1);
	i = 0;
	while (tmp != NULL)
	{
		if (tmp->token == WORD && ft_strcmp(tmp->content, ""))
			args = is_word(tmp->content, args, g_env, &i);
		if(tmp->token == REDIR_OUT || tmp->token == APPEND || tmp->token == REDIR_IN || tmp->token == HEARDOC)
		{
			fd = parse_redirection(tmp->token, tmp->next->content, fd,g_env);
			tmp = tmp->next;
		}
		if (!tmp->next || tmp->token == PIPE_LINE)
		{
			i = create(args, cmd, fd, i);
			if (tmp->next && tmp->token == PIPE_LINE)
				args = realloc_args(args, calculate_args(tmp->next) + 1);
		}
		tmp = tmp->next;
	}
}

void parse_args(t_lexer **list,t_command **cmd,  t_env **g_env)
{
	t_lexer *tmp;
	char **args;

	tmp = *list;
	int count;
	count = calculate_args(tmp);
	tmp = *list;
	args = malloc(sizeof(char *) * (count + 1));
	if(!args)
		return ;
	parsing1(tmp, args, g_env, cmd);

}


// void parsing(t_lexer **list, t_command **cmd, t_env **g_env)
// {
// 	t_lexer *tmp = *list;
// 	int i = 0;
// 	int j = 0;
// 	t_fd fd;
// 	int k;

// 	*cmd = NULL;
// 	fd.fd_in = 0;
// 	fd.fd_out = 1;
// 	char **args;
// 	char **p;
// 	int count = calculate_args(tmp);

// 	args = malloc(sizeof(char *) * (count + 1));
// 	if(!args)
// 		return ;

// 	while (tmp != NULL)
// 	{
// 		if (tmp->token == WORD)
// 		{
// 			j = parse_env(g_env, tmp->content);
// 			if(check_space(tmp->content) && j)
// 			{
// 				tmp->content = del_quote(tmp->content, '\'', '\"');
// 				p  = ft_split1(tmp->content, "' ' '\t' '\n' '\v' '\f' '\r'");
// 				k = 0;
// 				while(p[k])
// 				{
// 					args[i++] = ft_strdup(p[k]);
// 					free(p[k++]);
// 				}
// 				free(p);
// 			}
// 			else
// 			{

// 				tmp->content = del_quote(tmp->content, '\'', '\"');
// 				args[i] = ft_strdup(tmp->content);
// 				printf("%p\n", args[i]);
// 				i++;
// 			}

// 		}
// 		else if (tmp->token == REDIR_IN)
// 		{
// 			tmp = tmp->next;
// 			fd.fd_in = open(tmp->content, O_RDONLY, 0644);
// 			if (fd.fd_in == -1)
// 			{
// 				printf("minishell: %s: No such file or directory\n", tmp->content);
// 				return;
// 			}
// 		}
// 		else if (tmp->token == REDIR_OUT)
// 		{
// 			tmp = tmp->next;
// 			tmp->content = del_quote(tmp->content, '\'', '\"');
// 			fd.fd_out = open(tmp->content, O_CREAT | O_WRONLY | O_TRUNC, 0644);
// 			if (fd.fd_out == -1)
// 			{
// 				printf("minishell: $s: ambiguous redirect\n");
// 				return;
// 			}
// 		}
// 		else if(tmp->token == HEARDOC)
// 		{
// 			tmp = tmp->next;
// 			fd.fd_in = open("/tmp/srfak", O_RDONLY, 0644);
// 		}
// 		else if (tmp->token == APPEND)
// 		{
// 			tmp = tmp->next;
// 			tmp->content = del_quote(tmp->content, '\'', '\"');
// 			fd.fd_out = open(tmp->content, O_CREAT | O_WRONLY | O_APPEND, 0644);
// 			if (fd.fd_out == -1)
// 			{
// 				printf("minishell: $s: ambiguous redirect\n");
// 				return;
// 			}
// 		}

// 			if (!tmp->next || tmp->token == PIPE_LINE)
// 			{
// 				args[i] = NULL;
// 				my_lstadd_back(cmd, ft_new(args, fd));
// 				if (tmp->next && tmp->token == PIPE_LINE)
// 				{
// 					tmp = tmp->next;
// 					count = calculate_args(tmp);
// 					args = malloc(sizeof(char *) * (count + 1));
// 					if(!args)
// 						return ;
// 					i = 0;

// 				}
// 				else
// 					tmp = tmp->next;

// 				i = 0;
// 			}
// 			else
// 				tmp = tmp->next;

// 	}

// }
int valid(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\"' || str[i] == '\'')
			return (1);
		i++;
	}
	return (0);
}

char *expand_heredoc(char *str, t_env **g_env)
{
	static char   *new;
	int i;

	i = 0;
	while (str[i])
	{
		if(str[i] == '$' && !is_digit(str[i + 1]) && str[i + 1] != '\'' && str[i + 1] != '$' && str[i + 1] != '\0' && !is_spaces(str[i + 1]) && str[i + 1] != '\"' )
		{
			if(str[i + 1] == '\"' && str[i + 2] == '\0')
				break;
			else
				str = handler(str, &i, g_env);
		}
		i++;
	}
	free(new);
	return (str);
}

int here_doc(char *str, char *line, t_env **g_env)
{
	int fd;
	int check;

	unlink("/tmp/srfak");
	fd = open("/tmp/srfak", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	check = valid(str);
	str = del_quote(str, '\'', '\"');
	while(1)
	{
		line = readline("heredoc>");
		if(ft_strcmp(line, str) == 0)
		{
			free(line);
			return (1);
			break ;
		}
		if (!check)
		line = expand_heredoc(line, g_env);
		ft_putendl_fd(line, fd);
		free(line);
	}
	close(fd);
	return (0);
}

void heredoc(t_lexer **lexer, t_env **g_env)
{

	char *line;
	line = NULL;

	t_lexer *tmp = *lexer;

	while(tmp != NULL)
	{
		if (tmp->token == HEARDOC)
		{
			tmp = tmp->next;
			here_doc(tmp->content, line, g_env);
		}
		tmp = tmp->next;
	}
}
