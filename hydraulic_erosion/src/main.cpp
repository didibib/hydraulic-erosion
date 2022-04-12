#include "hepch.h"
#include "window.h"
#include "shallow_erosion.h"
#include "droplet_erosion.h"

using namespace he;

Window& window = Window::getInstance();
Program* hydra = new ShallowErosion();

int main(int argc, char* argv[])
{
	logger::init();

	window.init(800, 600, "Hydraulic Erosion");
	hydra->init();

	window.run(hydra);

	logger::shutdown();
	delete hydra;
}