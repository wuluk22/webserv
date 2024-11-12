NAME	= webserv

CC	= c++
FLAGS	= -g -std=c++98
RM	= rm -rf

OBJDIR	= .objs
FILES	= src/configuration_file_parsing/ConfigException src/configuration_file_parsing/ConfigParser src/configuration_file_parsing/ServerConfig src/configuration_file_parsing/ConfigParserUtils src/configuration_file_parsing/TokenCounter
SRC	= $(FILES:=.cpp)
OBJ	= $(addprefix $(OBJDIR)/, $(FILES:=.o))
DEPS	= $(OBJ:.o=.d)
HEADER	= src/configuration_file_parsing/ConfigException.hpp src/configuration_file_parsing/ConfigParser.hpp src/configuration_file_parsing/ServerConfig.hpp

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
