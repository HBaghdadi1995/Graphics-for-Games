#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main() {
	Window w("Coursework", 1920, 1080, true); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
			renderer.lastScene();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
			renderer.nextScene();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
			renderer.TogglePause();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
			renderer.ToggleFrameRate();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
			renderer.ToggleAutocam();
		}
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}