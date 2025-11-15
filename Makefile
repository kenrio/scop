# **************************************************************************** #
# VARIABLES


NAME		:= scop

CXX_C		:= c++
CXX_FLAGS	:= -Wall -Wextra -Werror

CXX_FLAGS	+= -g3 -O0

SRC_DIR		:= src
BIN_DIR		:= bin
INC_DIR		:= include


# **************************************************************************** #
# SOURCES 


SRC		:= \
		main.cpp \
		shader.cpp \

BIN		:= \
		$(addprefix $(BIN_DIR)/, \
		$(SRC:.cpp=.o))


# **************************************************************************** #
# LIBRARIES & FRAMEWORKS


GL_FLAGS	:= -lGLEW -lGL -lGLU
GLFW_FLAGS	:= -lglfw

X11_FLAGS	:= -lX11 

LDFLAGS		:= $(GL_FLAGS) $(GLFW_FLAGS) $(X11_FLAGS) -lm


# **************************************************************************** #
# INCLUDE


INCLUDE		:= -I$(INC_DIR)


# **************************************************************************** #
# RULES


all: $(NAME)

run: all
	./$(NAME)

$(NAME): $(BIN)
	$(CXX_C) $(CXX_FLAGS) $(BIN) $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_C) $(CXX_FLAGS) $(INCLUDE) -c $< -o $@

clean:
	$(RM) -rf $(BIN_DIR)
	# ${MAKE} -C $(MLX_DIR) clean

fclean: clean
	$(RM) $(NAME)

re: fclean all

glinfo:
	@glxinfo | grep "OpenGL version"
	@glxinfo | grep "OpenGL renderer"


# **************************************************************************** #
# PHONY


.PHONY: all run clean fclean re glinfo
