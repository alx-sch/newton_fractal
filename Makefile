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

# HEADER DIR
HEADER_DIR :=	include

# ISPC Compiler and files; flag '-O2' for standard optimization,
# target uses a SIMD instruction set supported by most modern CPUs
ISPC :=			ispc
ISPC_FLAGS :=	-O2 --target=avx2-i32x8
ISPC_SRC :=		$(SRCS_DIR)/fractal.ispc
ISPC_OBJ :=		$(OBJS_DIR)/fractal.o
ISPC_HEADER :=  $(HEADER_DIR)/fractal.h

# COMPILER
CXX :=			c++
CXXFLAGS +=		-Werror -Wextra -Wall
CXXFLAGS +=		-std=c++17
CXXFLAGS +=		-MMD -MP	# For dependency files
CXXFLAGS +=		-I$(HEADER_DIR)

# FORMATTING
BOLD :=			\033[1m
YELLOW :=		\033[33m
RED :=			\033[91m
GREEN :=		\033[92m
RESET :=		\033[0m

# BUILD MESSAGES
MSG_PARALLEL :=		Built for $(BOLD)$(GREEN)parallel ISPC execution (SIMD)$(RESET).
MSG_SEQUENTIAL :=	Built for $(BOLD)$(GREEN)sequential CPU execution (non-ISPC)$(RESET).
MSG_BUILD :=		$(MSG_PARALLEL)

# OUTPUT FILES
PPM_FILES :=	$(wildcard $(OUT_DIR)/*.ppm)
PNG_FILES :=	$(patsubst $(OUT_DIR)/%.ppm, $(OUT_DIR)/%.png, $(PPM_FILES))

###########
## RULES ##
###########

## Build executable ##

all:	$(NAME)

$(NAME):	$(OBJS) $(ISPC_OBJ) | $(OUT_DIR)
	@echo "$(YELLOW)Linking...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) $(ISPC_OBJ) -o $(NAME)
	@echo "$(YELLOW)$(BOLD)\n$(NAME)$(RESET) successfully compiled."
	@echo "$(MSG_BUILD)"
	@echo "$(BOLD)$(YELLOW)\nUsage:$(RESET)$(BOLD) ./$(NAME) <n> [width] [height]$(RESET)"

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# Rule for C++ objects / make them depend on ISPC header
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(ISPC_HEADER) | $(OBJS_DIR)
	@echo "$(YELLOW)Compiling$(RESET) $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	@mkdir -p $@

# Rule for ISPC object (generates .o and .h files)
$(ISPC_OBJ) $(ISPC_HEADER): $(ISPC_SRC) | $(OBJS_DIR)
	@echo "$(YELLOW)Compiling$(RESET) $<...$(RESET)"
	@$(ISPC) $(ISPC_FLAGS) $< -o $(ISPC_OBJ) -h $(ISPC_HEADER) -I$(HEADER_DIR)

## Build using sequential CPU execution (non-ISPC) ##

seq: CXXFLAGS += -DSEQ
seq: MSG_BUILD := $(MSG_SEQUENTIAL)
seq: re

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

debug_seq:	CXXFLAGS += -DDEBUG -g
debug_seq:	seq

## Clean up ##

clean:
	@rm -rf $(OBJS_DIR)
	@rm -f $(ISPC_HEADER)
	@echo "$(RED)$(NAME) object files removed.$(RESET)"

fclean:	clean
	@rm -f $(NAME)
	@rm -rf $(OUT_DIR)
	@echo "$(RED)$(NAME) and output files removed.$(RESET)"

re:	fclean all

.PHONY: all clean fclean re debug png seq debug_seq

-include $(DEPS)
