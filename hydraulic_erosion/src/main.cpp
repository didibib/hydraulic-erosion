#include "hepch.h"
#include "window.h"
#include "hydraulic_erosion.h"

using namespace he;

Window& window = Window::getInstance();
Program* hydra = new HydraulicErosion();

int main(int argc, char* argv[])
{
	logger::init();

	window.init(800, 600, "Hydraulic Erosion");
	hydra->init();

	window.run(hydra);

	logger::shutdown();
	delete hydra;
}