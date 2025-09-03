

#include "minishell.h"

int write_error_and_return(char *msg, int error)
{
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    perror(msg);
    return (error);
}

int execute_redirection_in(t_ast *ast)
{
    int infile_fd;

    infile_fd = open(*ast->value, O_RDONLY);
    if (infile_fd < 0)
        return (write_error_and_return(*ast->value, errno));
    if (dup2(infile_fd, STDIN_FILENO) == -1)
    {
        close(infile_fd);
        return (write_error_and_return("dup2", errno));
    }
    close(infile_fd);
    return (0);
}

int execute_redirection_out(t_ast *ast, t_token_type type)
{
    int outfile_fd;

    if (type == OUT)
        outfile_fd = open(*ast->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else
        outfile_fd = open (*ast->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (outfile_fd < 0)
        return (write_error_and_return(*ast->value, errno));
    if (dup2(outfile_fd, STDOUT_FILENO) == -1)
    {
        close(outfile_fd);
        return (write_error_and_return("dup2", errno));
    }
    close(outfile_fd);
    return (0);
}
// update error functions: 
int   check_redirection(t_ast *ast, t_shell *shell)
{
    pid_t   pid;

    pid = fork();
    if (pid == -1)
        return (write_error_and_return("fork", errno));
    if (pid == 0)
    {
        if (ast->type ==IN)
            shell->exit_code = execute_redirection_in(ast->right);
        if (ast->type == OUT || ast->type == APPEND)
            shell->exit_code = execute_redirection_out(ast->right, ast->type);
// add other redirections latwr 
        if (shell->exit_code != 0)
            exit(shell->exit_code);
        exit(execute_ast(ast->left, shell));
    }
    waitpid(pid, &shell->exit_code, 0);
	if (WIFEXITED(shell->exit_code))
		return (WEXITSTATUS(shell->exit_code));
	return (EXIT_FAILURE);
}

/*
1. fork, check fork, exit if breaks
2. check what type of redirection
3. execute the child
4. check what it returns, if not success, stop everything
4. parent waits for a child and updates the execution
*/