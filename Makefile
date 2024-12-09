NAME	= webserv

CC	= c++
FLAGS	= -g -std=c++98
RM	= rm -rf

OBJDIR	= .objs
FILES	=	src/main \
			src/ServerHandler \
			src/ServerBase \
			src/HttpRequestHandler \
			src/HttpResponseHandler \
			src/HttpRequestUtils \
			src/HttpRequestParsing \
			src/HttpRequestDir \
			src/DirectoryHandler \
			src/RequestResponseState \
			src/Logger \
			src/ErrorHandler \
			src/configuration_file_parsing/ConfigException \
			src/configuration_file_parsing/PathValidator \
			src/configuration_file_parsing/ConfigParser \
			src/configuration_file_parsing/ServerConfig \
			src/configuration_file_parsing/ConfigParserUtils \
			src/configuration_file_parsing/TokenCounter \
			src/configuration_file_parsing/DirectiveParser 
SRC	= $(FILES:=.cpp)
OBJ	= $(addprefix $(OBJDIR)/, $(FILES:=.o))
DEPS	= $(OBJ:.o=.d)
HEADER	=	src/ServerHandler.hpp \
			src/ServerHandler.hpp \
			src/HttpRequestHandler.hpp \
			src/HttpResponseHandler.hpp \
			src/RequestResponseState.hpp \
			src/Logger.hpp \
			src/ErrorHandler.hpp \
			src/configuration_file_parsing/ConfigException.hpp \
			src/configuration_file_parsing/ConfigParser.hpp \
			src/configuration_file_parsing/ServerConfig.hpp \
			src/configuration_file_parsing/PathValidator.hpp

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
