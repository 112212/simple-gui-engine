#include "../UI/Gui.hpp"
#include "../UI/AllControls.hpp"
#include "../UI/backends/default/Backend.hpp"

#include <iostream>
#include <thread>
#include <unistd.h>

ng::Gui gui;

using namespace std;
using namespace ng;

GUIFUNC(say) {
	if(args.cmd_args.empty()) return;
	string l;
	for(auto s : args.cmd_args) {
		l += s + " ";
	}
	cout << "control " << args.control->GetId() << " says: " << l << endl;
}


int main() {
	int sizeX = 1800;
	int sizeY = 900;

	ng::DefaultBackend::SetupWindow("basic", 100, 100, sizeX, sizeY);
	std::cout << "size of control: " << sizeof(Control) << "\n";
	
	gui = Gui(sizeX, sizeY);
	ng::DefaultBackend::SetBackend(&gui);
	
	try {
		gui.LoadXml("data/basic.xml");
	} catch (std::exception e) {
		return 0;
	}
	
	double t=25;
	gui.OnRender([&]() {
		Point pt = gui.GetCursor().GetPosition();
		// tbox->SetText(std::to_string(pt.x) + " " + std::to_string(pt.y));
		// if (gui.GetActiveControl())
		// std::cout << gui.GetActiveControl()->GetId() << "\n";
		// std::cout << gui.GetActiveControl() << " : " << gui.GetSelectedControl() << " : " << gui.IsKeyboardLocked() << "\n";
		t+=2;
	});
	
	ng::DefaultBackend::MainLoop(&gui);
}
