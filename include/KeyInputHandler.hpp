#ifndef KEYINPUTHANDLER_HPP
# define KEYINPUTHANDLER_HPP

#include <GLFW/glfw3.h>

class	KeyInputHandler
{

public:

	KeyInputHandler(GLFWwindow *window);

	void	update(void);
	bool	isPressed(int key) const;
	bool	shouldClose(void) const;

private:

	GLFWwindow	*window;

};

#endif
