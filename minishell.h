#ifndef MINISHELL_H
# define MINISHELL_H
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>

typedef enum s_tokens
{
	WORD,
	PIPE_LINE ,
	REDIR_IN ,
	REDIR_OUT,
	HEARDOC ,
	APPEND ,
} 	t_tokens;

typedef struct s_fd
{
	int		fd_in;
	int		fd_out;
}	t_fd;


typedef struct s_redir_list
{
	char	*file_name;
	t_tokens	type;
	struct s_redir_list	*next;
}	t_redir_list;


typedef struct s_command
{
	char	**args;
	t_fd	fd;
	t_redir_list	*redir_list;
}	t_command;



typedef struct s_lexer
{
	char    	*content;
	t_tokens        token;
	int		i;
	struct s_lexer	*next;
	struct s_lexer	*prev;
}	t_lexer;

typedef struct s_env
{
	char	*key;
	char	*value;
	struct s_env	*next;
	struct s_env	*prev;
}	t_env;

enum command_type
{
	ls,
	pwd,
	cd,
	echo,
	env,
	export,
	unset,
	exit1,
};

void craete_env(char **env_list, t_env **g_env);
char **ft_split(char const *s, char c);
char	*ft_strdup(const char *s1);
char	*ft_substr(char const *s, unsigned int start, size_t len);
size_t	ft_strlen(const char *s);
int	ft_strncmp( char *s1, char *s2, size_t n);
void ft_lstadd_back(t_lexer **lst, t_lexer *new);
t_lexer *ft_lstnew(char *content, t_tokens type);


char *expand_variables(t_lexer **list, t_env **p_env);
void free_list(t_lexer *lst);
void expand(t_lexer **list, t_env **g_env);


int parsing(t_lexer **list, t_command **command);

#endif
