NAME :=			newton_fractal

# SOURCE FILES
SRCS_DIR :=		src
SRCS_FILES :=	main.cpp \
				Args.cpp

SRCS :=			$(SRCS_FILES:%.cpp=$(SRCS_DIR)/%.cpp)

# OBJECT FILES
OBJS_DIR :=		obj
OBJS :=			$(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)
DEPS :=			$(OBJS:.o=.d)

# COMPILER
CXX :=			c++
CXXFLAGS +=		-Werror -Wextra -Wall
CXXFLAGS +=		-MMD -MP # For dependency files
CXXFLAGS +=		-Wpedantic

# FORMATTING

RESET :=		\033[0m
BOLD :=			\033[1m
YELLOW :=		\033[33m
RED :=			\033[91m

###########
## RULES ##
###########

all:	$(NAME)

$(NAME):	$(OBJS)
	@echo "$(YELLOW)Linking...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(YELLOW)\n$(NAME)$(RESET) successfully compiled."
	@echo "$(BOLD)\nUsage: ./$(NAME) <n> [width] [height]$(RESET)"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	@echo "$(YELLOW)Compiling$(RESET) $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJS_DIR)
	@echo "$(RED)$(NAME) object files removed.$(RESET)"

fclean:	clean
	@rm -f $(NAME)
	@echo "$(RED)$(NAME) removed.$(RESET)"

re:	fclean all

.PHONY: all clean fclean

-include $(DEPS)
