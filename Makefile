NAME :=			newton_fractal

# OUTPUT FOLDER
OUT_DIR :=		out

# SOURCE FILES
SRCS_DIR :=		src
SRCS_FILES :=	main.cpp \
				Args.cpp \
				Fractal.cpp \
				complexMath.cpp

SRCS :=			$(SRCS_FILES:%.cpp=$(SRCS_DIR)/%.cpp)

# OBJECT FILES
OBJS_DIR :=		obj
OBJS :=			$(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)
DEPS :=			$(OBJS:.o=.d)

# COMPILER
CXX :=			c++
CXXFLAGS +=		-Werror -Wextra -Wall
CXXFLAGS +=		-std=c++17
CXXFLAGS +=		-MMD -MP	# For dependency files
CXXFLAGS +=		-Iinclude	# use "include" directory for header files

# FORMATTING
BOLD :=			\033[1m
YELLOW :=		\033[33m
RED :=			\033[91m
RESET :=		\033[0m

# OUTPUT FILES
PPM_FILES :=	$(wildcard $(OUT_DIR)/*.ppm)
PNG_FILES :=	$(patsubst $(OUT_DIR)/%.ppm, $(OUT_DIR)/%.png, $(PPM_FILES))

###########
## RULES ##
###########

## Build executable ##

all:	$(NAME)

$(NAME):	$(OBJS) | $(OUT_DIR)
	@echo "$(YELLOW)Linking...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(YELLOW)$(BOLD)\n$(NAME)$(RESET) successfully compiled."
	@echo "$(BOLD)$(YELLOW)\nUsage:$(RESET)$(BOLD) ./$(NAME) <n> [width] [height]$(RESET)"

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	@echo "$(YELLOW)Compiling$(RESET) $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	@mkdir -p $@

## Convert PPM to PNG ##

png:	$(PNG_FILES)
	@echo "$(BOLD)All PPM files converted to PNG.$(RESET)"

# 'convert' might trigger a warning as it's depraciated in newer versions of Imagemagick
# Silence this warning
$(OUT_DIR)/%.png: $(OUT_DIR)/%.ppm
	@echo "Converting '$(YELLOW)$<${RESET}' to '$(YELLOW)$@${RESET}'..."
	@convert $< $@ 2> /dev/null
	@echo "  ...Deleting '$(YELLOW)$<$(RESET)'"
	@rm -f $<

## Debug build; logs extra info ##

debug:	CXXFLAGS += -DDEBUG -g
debug:	re
	@echo "\n$(BOLD)Debug prints enabled.$(RESET)"

## Clean up ##

clean:
	@rm -rf $(OBJS_DIR)
	@echo "$(RED)$(NAME) object files removed.$(RESET)"

fclean:	clean
	@rm -f $(NAME)
	@rm -rf $(OUT_DIR)
	@echo "$(RED)$(NAME) and output files removed.$(RESET)"

re:	fclean all

.PHONY: all clean fclean re debug png

-include $(DEPS)
