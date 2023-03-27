#include "hepch.h"
#include "window.h"
#include "shallow_erosion.h"
#include "droplet_erosion.h"

using namespace he;

Window& window = Window::getInstance();
Program* droplet = new DropletErosion();
Program* shallow = new ShallowErosion();

int main(int argc, char* argv[])
{
	logger::init();

	window.init(800, 600, "Hydraulic Erosion");

	window.run(droplet);

	logger::shutdown();
	delete shallow;
}