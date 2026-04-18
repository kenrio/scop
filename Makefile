# **************************************************************************** #
# VARIABLES


NAME		:= scop

UNAME		:= $(shell uname -s)

CXX_C		:= c++
CXX_FLAGS	:= -Wall -Wextra -Werror -std=c++17 -pedantic

CC			:= gcc
C_FLAGS		:= -Wall -Wextra -Werror

SRC_DIR		:= src
BIN_DIR		:= bin
INC_DIR		:= include


# **************************************************************************** #
# SOURCES 


SRC			:= \
			main.cpp \
			Vec3.cpp \
			Vec4.cpp \
			Mat4.cpp \
			ObjParser.cpp \
			KeyInputHandler.cpp \
			Shader.cpp \
			Texture.cpp \
			ModelViewer.cpp \

IMGUI_SRC	:= \
			imgui/imgui.cpp \
			imgui/imgui_draw.cpp \
			imgui/imgui_tables.cpp \
			imgui/imgui_widgets.cpp \
			imgui/imgui_demo.cpp \
			imgui/imgui_impl_glfw.cpp \
			imgui/imgui_impl_opengl3.cpp \

C_SRC		:= \
			glad.c \

BIN			:= \
			$(addprefix $(BIN_DIR)/, \
			$(SRC:.cpp=.o))

IMGUI_BIN	:= \
			$(addprefix $(BIN_DIR)/, \
			$(IMGUI_SRC:.cpp=.o))

C_BIN		:= \
			$(addprefix $(BIN_DIR)/, \
			$(C_SRC:.c=.o))


# **************************************************************************** #
# LIBRARIES & FRAMEWORKS


GL_FLAGS	:= -framework OpenGL
GLFW_FLAGS	:= -lglfw

LDFLAGS		:= -L/opt/homebrew/lib $(GL_FLAGS) $(GLFW_FLAGS)


# **************************************************************************** #
# INCLUDE


INCLUDE		:= -I$(INC_DIR) -I$(INC_DIR)/imgui -I/opt/homebrew/include


# **************************************************************************** #
# RULES


all: $(NAME)

run: all
	./$(NAME)

$(NAME): $(BIN) $(IMGUI_BIN) $(C_BIN)
	$(CXX_C) $(CXX_FLAGS) $(BIN) $(IMGUI_BIN) $(C_BIN) $(LDFLAGS) -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_C) $(CXX_FLAGS) $(INCLUDE) -c $< -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(INCLUDE) -c $< -o $@

clean:
	$(RM) -rf $(BIN_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

glinfo:
	@system_profiler SPDisplaysDataType | grep -E "(OpenGL|Metal)"


# **************************************************************************** #
# PHONY


.PHONY: all run clean fclean re glinfo
