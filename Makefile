NAME	= webserv

CC	= c++
FLAGS	= -Wall -Werror -Wextra -std=c++98
RM	= rm -rf

OBJDIR	= .objs
FILES	=	src/main \
			src/ServerHandler \
			src/ServerBase \
			src/HttpRequestHandler \
			src/HttpResponseHandler \
			src/Logger \
			src/ErrorHandler \
			src/HttpRequestUtils \
			src/HttpRequestDir \
			src/HttpRequestParsing \
			src/DirectoryHandler \
			src/RequestResponseState
SRC	= $(FILES:=.cpp)
OBJ	= $(addprefix $(OBJDIR)/, $(FILES:=.o))
DEPS	= $(OBJ:.o=.d)
HEADER	=	src/ServerHandler.hpp \
			src/ServerHandler.hpp \
			src/HttpRequestHandler.hpp \
			src/HttpResponseHandler.hpp \
			src/Logger.hpp \
			src/ErrorHandler.hpp \
			src/DirectoryHandler.hpp \
			src/RequestResponseState.hpp

GREEN	= \e[92m
YELLOW	= \e[93m
BLUE	= \e[94m
RESET	= \e[0m

all:	$(NAME)

$(NAME):	$(OBJ) $(HEADER)
	@$(CC) $(OBJ) -o $(NAME)
	@printf "$(GREEN)- $(NAME) is ready\n$(RESET)"

run:	all
	./$(NAME)

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "$(BLUE)- .o files removed\n$(RESET)"

fclean:	clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)- program deleted\n$(RESET)"

re: fclean all
