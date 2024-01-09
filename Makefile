NAME = webserv

SRC = $(shell find . -name '*.cpp')
INC = $(shell find . -name '*.hpp')

OBJ_DIR = compiled
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -std=c++98 -fsanitize=address -g

GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m
RM = rm -f

all: Start

$(OBJ_DIR)/%.o: %.cpp $(INC)
	@mkdir -p $(dir $@)
	$(CPP) ${CPPFLAGS} -c $< -o $@

-include $(OBJ:.o=.d)

$(NAME): $(OBJ)
	@echo "$(GREEN)Compiling...$(NC)"
	@$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ)
	@echo "$(GREEN)Compiled.$(NC)"

clean:
	@$(RM) $(OBJ)
	@$(RM) -rf $(OBJ_DIR)
	@echo "$(RED)Cleaned up object files!$(NC)"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) -rf $(OBJ_DIR)
	@echo "$(RED)Removed executable!$(NC)"

Start: $(NAME)
	clear && ./$(NAME) conf/serv.conf


re: fclean all
