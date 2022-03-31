#include "hepch.h"
#include "window.h"
#include "hydraulic_erosion.h"

using namespace he;

Window& window = Window::getInstance();
Program* hydra = new HydraulicErosion();

int main(int argc, char* argv[])
{
	window.init(800, 600, "HE");

	window.run(hydra);

	delete hydra;
}