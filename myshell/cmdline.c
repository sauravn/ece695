/* 
 * Skeleton code for commandline parsing for Lab 1 - Shell processing
 * This file contains the skeleton code for parsing input from the command
 * line.
 * Acknowledgement: UCLA CS111
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include "cmdline.h"

/*
 * parsestate_t
 *
 *   The parsestate_t object represents the current state of the command line
 *   parser.  'parse_init' initializes a parsestate_t object for a command
 *   line, then calls to 'parse_gettoken' step through the command line one
 *   token at a time.  'parse_ungettoken' backs up one token.
 */


/*
 * parse_init(parsestate, line)
 *
 *   Initialize a parsestate_t object for a given command line.
 */

void
parse_init(parsestate_t *parsestate, char *input_line)
{
	parsestate->position = input_line;
	parsestate->last_position = NULL;
}


/*
 * parse_gettoken(parsestate, token)
 *
 *   Fetches the next token from the input line.
 *   The token's type is stored in 'token->type', and the token itself is
 *   stored in 'token->buffer'.  The parsestate itself is moved past the
 *   current token, so that the next call to 'parse_gettoken' will return the
 *   next token.
 *   Tokens are delimited by space characters. Any leading space is skipped.
 *
 *   EXERCISES:
 *        1. The example function just reads the whole command line into a
 *           single token (it ignores spaces). Make it stop when it reaches the
 *           end of a token, as delimited by "whitespace" (a C term for
 *           blank characters, including tab '\t' and newline '\n'.  Hint:
 *           man isspace)
 *        2. Add support for "double quotes".  If the parsestate contains
 *           the string ==> "a b" c <== (arrows not included), then the next
 *           call to 'parse_gettoken' should stick the string "a b" into
 *           'token', and move the parsestate to "c" or " c".
 *   EXTRA CREDIT EXERCISE:
 *        3. Allow special characters like ';' and '&' to terminate a token.
 *           In real shells, "a;b" is treated the same as "a ; b".
 *           The following characters and two-character sequences should
 *           end the current token, except if they occur in double quotes.
 *           Of course, you can return one of these sequences as a token
 *           when it appears first in the string.
 *                (  )  ;  |  &  &&  ||  >  <
 *           Note that "2>" does not end a token.  The string "x2>y" is parsed
 *           as "x2 > y", not "x 2> y".
 */

void
parse_gettoken(parsestate_t *parsestate, token_t *token)
{
	int i;
	char *str = parsestate->position;	// current string
	int quote_state;			// Hint!
	int any_quotes;				// Hint!

	// EXERCISE: Skip initial whitespace in 'str'.

	/* Your code here. */
	while (isspace(*str))
		str++;
	
	// Report TOK_END at the end of the command string.
	if (*str == '\0') {
		// Save initial position so parse_ungettoken() will work
		parsestate->last_position = parsestate->position;
		token->buffer[0] = '\0';	// empty token
		token->type = TOK_END;
		return;
	}


	// EXERCISE: Store the next token into 'token', and terminate the token
	// with a null character.  Handle quotes properly.  Store at most
	// TOKENSIZE - 1 characters into 'token' (plus a terminating null
	// character); longer tokens should cause an error.

	// The current code treats the entire rest of the string as a token.
	// Change this code to handle quotes and terminate tokens at spaces.

	i = 0;
	quote_state = QUOTE_OUT;
	any_quotes = QUOTE_OUT;

	//while (*str != '\0' && !isspace(*str)) {
	while (*str != '\0') {
		if (i >= TOKENSIZE - 1)
			// Token too long; this is an error
			goto error;
		while (*str == '"') {
			quote_state = - quote_state;
			any_quotes = QUOTE_IN;
			str++;
		}

		if ((quote_state == QUOTE_OUT) && isspace(*str))
			break;
		else
			token->buffer[i++] = *str++;
	}
	token->buffer[i] = '\0';	// end the token string


	// Save initial position so parse_ungettoken() will work
	parsestate->last_position = parsestate->position;
	// Move current position in place for the next token
	parsestate->position = str;


	// EXERCISE: Examine the token and store its type in token->type.
	// Quoted special tokens, such as '">"', have type TOK_NORMAL.
	// done by get_token_type func

	/* Your code here. */
	
	//token->type = TOK_NORMAL;
	if (any_quotes == QUOTE_IN)
		token->type = TOK_NORMAL;
	else
		token->type = get_token_type(token->buffer);
	return;

 error:
	token->buffer[0] = '\0';
	token->type = TOK_ERROR;
}

tokentype_t get_token_type(const char* token)
{
	switch (*token) {
	case '<':
		return TOK_LESS_THAN;
	case '>':
		return TOK_GREATER_THAN;
	case '2':
		if (*(token+1) == '>')
			return TOK_2_GREATER_THAN;
		else
			return TOK_NORMAL;
	case ';':
		return TOK_SEMICOLON;
	case '&':
		if (*(token+1) == '&')
			return TOK_DOUBLEAMP;
		else
			return TOK_AMPERSAND;
	case '|':
		if (*(token+1) == '|')
			return TOK_DOUBLEPIPE;
		else
			return TOK_PIPE;
	case '(':
		return TOK_OPEN_PAREN;
	case ')':
		return TOK_CLOSE_PAREN;
	default:
		return TOK_NORMAL;
	}
}


/*
 * parse_ungettoken(parsestate)
 *
 *   Backs up the parsestate by one token.
 *   It's impossible to back up more than one token; if you call
 *   parse_ungettoken() twice in a row, the second call will fail.
 */

void
parse_ungettoken(parsestate_t *parsestate)
{
	// Can't back up more than one token.
	assert(parsestate->last_position != NULL);
	parsestate->position = parsestate->last_position;
	parsestate->last_position = NULL;
}


/*
 * command_alloc()
 *
 *   Allocates and returns a new blank command.
 */

command_t *
command_alloc(void)
{
	// Allocate memory for the command
	command_t *cmd = (command_t *) malloc(sizeof(*cmd));
	if (!cmd)
		return NULL;
	
	// Set all its fields to 0
	memset(cmd, 0, sizeof(*cmd));

	return cmd;
}


/*
 * command_free()
 *
 *   Frees all memory associated with a command.
 *
 *   EXERCISE:
 *        Fill in this function.
 *        Also free other structures pointed to by 'cmd', including
 *        'cmd->subshell' and 'cmd->next'.
 *        If you're not sure what to free, look at the other code in this file
 *        to see when memory for command_t data structures is allocated.
 */

void
command_free(command_t *cmd)
{
	int i;
	
	// It's OK to command_free(NULL).
	if (!cmd)
		return;

	/* Your code here. */
}


/*
 * command_parse(parsestate)
 *
 *   Parses a single command_t structure from the input string.
 *   Returns a pointer to the allocated command, or NULL on error
 *   or if the command is empty. (One example is if the end of the
 *   line is reached, but there are other examples too.)
 *
 *   EXERCISES:
 *        The current version of the function just grabs all the tokens
 *        from 'input' and doesn't stop on command-delimiting tokens.
 *        Your job is to enhance its error checking (to avoid exceeding
 *        the maximum number of tokens in a command, for example), to make it
 *        stop at the end of the command, and to handle parentheses and
 *        redirection correctly.
 */

command_t *
command_parse(parsestate_t *parsestate)
{
	int i = 0;
	command_t *cmd = command_alloc();
	if (!cmd)
	{
		printf("cmd alloc error\n");
		return NULL;
	}

	while (1) {
		// EXERCISE: Read the next token from 'parsestate'.

		// Normal tokens go in the cmd->argv[] array.
		// Redirection file names go into cmd->redirect_filename[].
		// Open parenthesis tokens indicate a subshell command.
		// Other tokens complete the current command
		// and are not actually part of it;
		// use parse_ungettoken() to save those tokens for later.

		// There are a couple errors you should check.
		// First, be careful about overflow on normal tokens.
		// Each command_t only has space for MAXTOKENS tokens in
		// 'argv'. If there are too many tokens, reject the whole
		// command.
		// Second, redirection tokens (<, >, 2>) must be followed by
		// TOK_NORMAL tokens containing file names.
		// Third, a parenthesized subcommand can't be part of the
		// same command as other normal tokens.  For example,
		// "echo ( echo foo )" and "( echo foo ) echo" are both errors.
		// (You should figure out exactly how to check for this kind
		// of error. Try interacting with the actual 'bash' shell
		// for some ideas.)
		// 'goto error' when you encounter one of these errors,
		// which frees the current command and returns NULL.

		// Hint: An open parenthesis should recursively call
		// command_line_parse(). The command_t structure has a slot
		// you can use for parens; figure out how to use it!

		if (i > MAXTOKENS)
			goto error;

		token_t token;
		parse_gettoken(parsestate, &token);

		switch (token.type) {
		case TOK_NORMAL:
			//printf("%s len: %d", token.buffer, strlen(token.buffer));
			if (cmd->subshell != NULL) {
				parse_ungettoken(parsestate);
				goto done;
			}
			if (strlen(token.buffer) != 0) {
				cmd->argv[i] = strdup(token.buffer);
				i++;
			}
			break;
		case TOK_LESS_THAN:
			//printf("input red\n");
			parse_gettoken(parsestate, &token);
			if (token.type != TOK_NORMAL)
				goto error;
			cmd->redirect_filename[0] = strdup(token.buffer);
						break;
		case TOK_GREATER_THAN:
			//printf("output red\n");
			parse_gettoken(parsestate, &token);
			if (token.type != TOK_NORMAL)
				goto error;
			cmd->redirect_filename[1] = strdup(token.buffer);
						break;
		case TOK_2_GREATER_THAN:
			//printf("stderr red\n");
			parse_gettoken(parsestate, &token);
			if (token.type != TOK_NORMAL)
				goto error;
			cmd->redirect_filename[2] = strdup(token.buffer);
			break;
		case TOK_OPEN_PAREN:
			cmd->subshell = command_line_parse(parsestate, PARENS_IN);
			//goto done;
			break;
		case TOK_CLOSE_PAREN:
		default:
			parse_ungettoken(parsestate);
			goto done;
		}
	}

 done:
	// NULL-terminate the argv list
	cmd->argv[i] = 0;

	// EXERCISE: Make sure you return the right return value!

	if (i == 0 && cmd->subshell == NULL) {
		/* Empty command */
		command_free(cmd);
		return NULL;
	} else
		return cmd;
	
 error:
	command_free(cmd);
	return NULL;
}


/*
 * command_line_parse(parsestate, in_parens)
 *
 *   Parses a command line from 'input' into a linked list of command_t
 *   structures. The head of the linked list is returned, or NULL is
 *   returned on error.
 *   If 'in_parens != 0', then command_line_parse() is being called recursively
 *   from command_parse().  A right parenthesis should end the "command line".
 *   But at the top-level command line, when 'in_parens == 0', a right
 *   parenthesis is an error.
 */
command_t *
command_line_parse(parsestate_t *parsestate, int in_parens)
{
	command_t *prev_cmd = NULL;
	command_t *head = NULL;
	command_t *cmd;
	token_t token;
	int r;

	// This loop has to deal with command syntax in a smart way.
	// Here's a non exhaustive list of the behavior it should implement
	// when 'in_parens == 0'.

	// COMMAND                             => OK
	// COMMAND ;                           => OK
	// COMMAND && COMMAND                  => OK
	// COMMAND &&                          => error (can't end with &&)
	// COMMAND )                           => error (but OK if "in_parens")
	
	while (1) {
		// Parse the next command.
		cmd = command_parse(parsestate);
		if (!cmd)		// Empty commands are errors.
			goto error;

		// Link the command up to the command line.
		if (prev_cmd)
			prev_cmd->next = cmd;
		else
			head = cmd;
		prev_cmd = cmd;

		// EXERCISE: Fetch the next token to see how to connect this
		// command with the next command.  React to errors with
		// 'goto error'.  The ";" and "&" tokens may require special
		// handling, since unlike other special tokens, they can end
		// the command line.
		parse_gettoken(parsestate, &token);
		if (token.type == TOK_NORMAL)
			printf("parse token error");
		cmd->controlop = token.type;
		/* Your code here */

		switch (token.type) {
		case TOK_SEMICOLON:
		case TOK_AMPERSAND:
			parse_gettoken(parsestate, &token);
			if (token.type != TOK_END)
				parse_ungettoken(parsestate);
			else
				goto done;
			break;
		case TOK_CLOSE_PAREN:
			if (in_parens == PARENS_IN) {
				cmd->controlop = CMD_END;
				goto done;
			} else
				goto error;
			break;
		case TOK_END:
			goto done;
		default:
			;
		}
	}

 done:
	// EXERCISE: Check that the command line ends properly.
/* 	 if (in_parens == PARENS_OUT) {
 		 if (strlen(parsestate->position) != 0) {
 			 printf("strlen %d\n", strlen(parsestate->position));
 			 perror("line end error");
 			 exit(1);
 		 }
 	 }*/
	/* Your code here */

	return head;

 error:
	command_free(head);
	return NULL;
}


/*
 * commandlist_print(command, indent)
 *
 *   Prints a representation of the command to standard output.
 */

void
command_print(command_t *cmd, int indent)
{
	int argc, i;
	
	if (cmd == NULL) {
		printf("%*s[NULL]\n", indent, "");
		return;
	}

	for (argc = 0; argc < MAXTOKENS && cmd->argv[argc]; argc++)
		/* do nothing */;

	// More than MAXTOKENS is an error
	assert(argc <= MAXTOKENS);

	printf("%*s[%d args", indent, "", argc);
	for (i = 0; i < argc; i++)
		printf(" \"%s\"", cmd->argv[i]);

	// Print redirections
	if (cmd->redirect_filename[STDIN_FILENO])
		printf(" <%s", cmd->redirect_filename[STDIN_FILENO]);
	if (cmd->redirect_filename[STDOUT_FILENO])
		printf(" >%s", cmd->redirect_filename[STDOUT_FILENO]);
	if (cmd->redirect_filename[STDERR_FILENO])
		printf(" 2>%s", cmd->redirect_filename[STDERR_FILENO]);

	// Print the subshell command, if any
	if (cmd->subshell) {
		printf("\n");
		command_print(cmd->subshell, indent + 2);
	}
	
	printf("] ");
	switch (cmd->controlop) {
	case TOK_SEMICOLON:
		printf(";");
		break;
	case TOK_AMPERSAND:
		printf("&");
		break;
	case TOK_PIPE:
		printf("|");
		break;
	case TOK_DOUBLEAMP:
		printf("&&");
		break;
	case TOK_DOUBLEPIPE:
		printf("||");
		break;
	case TOK_END:
		// we write "END" as a dot
		printf(".");
		break;
	default:
		assert(0);
	}

	// Done!
	printf("\n");

	// if next is NULL, then controlop should be CMD_END, CMD_BACKGROUND,
	// or CMD_SEMICOLON
	assert(cmd->next || cmd->controlop == CMD_END
	       || cmd->controlop == CMD_BACKGROUND
	       || cmd->controlop == CMD_SEMICOLON);

	if (cmd->next)
		command_print(cmd->next, indent);
}
