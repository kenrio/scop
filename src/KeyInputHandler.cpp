#include "KeyInputHandler.hpp"

KeyInputHandler::KeyInputHandler(GLFWwindow *window) : window(window) {}

void	KeyInputHandler::update(void)
{
	glfwPollEvents();
}

bool	KeyInputHandler::isPressed(int key) const
{
	return (glfwGetKey(window, key) == GLFW_PRESS);
}

bool	KeyInputHandler::shouldClose(void) const
{
	return (glfwWindowShouldClose(window));
}
