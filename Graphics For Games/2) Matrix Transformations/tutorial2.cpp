#pragma comment(lib, "nclgl.lib")

#include "../../nclGL/window.h"
#include "Renderer.h"

int main() {
	Window w("Vertex Transformation!", 800, 600, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	float scale = 100.0f;
	float rotation = 0.0f;
	Vector3 position(0, 0, -1500.0f);
	float fov = 45.0f;

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_ESCAPE)) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_1))
			renderer.SwitchToOrthographic();
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_2))
			renderer.SwitchToPerspective();

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_PLUS))
			++scale;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_MINUS))
			--scale;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_LEFT))
			++rotation;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_RIGHT))
			--rotation;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_K))
			position.y -= 0.3f;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_I))
			position.y += 0.3f;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_J))
			position.x -= 0.3f;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_L))
			position.x += 0.3f;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_O))
			position.z -= 0.3f;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_P))
			position.z += 0.3f;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_Z))
			fov -= 0.1f;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::KEYBOARD_X))
			fov += 0.1f;

		renderer.SetRotation(rotation);
		renderer.SetScale(scale);
		renderer.SetPosition(position);
		renderer.setFov(fov);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}