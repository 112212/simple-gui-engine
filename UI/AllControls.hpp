#ifndef ALL_CONTROLS_HPP
#define ALL_CONTROLS_HPP
#ifdef USE_SDL
	#include "controls/SDL/Button.hpp"
	#include "controls/SDL/Canvas.hpp"
	#include "controls/SDL/CheckBox.hpp"
	#include "controls/SDL/ComboBox.hpp"
	#include "controls/SDL/Container.hpp"
	#include "controls/SDL/Label.hpp"
	#include "controls/SDL/ListBox.hpp"
	#include "controls/SDL/TextBox.hpp"
	#include "controls/SDL/ScrollBar.hpp"
	#include "controls/SDL/TabContainer.hpp"
	#include "controls/SDL/Terminal.hpp"
	#include "controls/SDL/TrackBar.hpp"
	#include "controls/SDL/Dialog.hpp"
	#include "controls/SDL/GridContainer.hpp"
	#include "controls/SDL/RadioButton.hpp"
	#include "controls/SDL/WidgetMover.hpp"
	#include "controls/SDL/WidgetResizer.hpp"
#elif USE_SFML
	#include "controls/SFML/Button.hpp"
	#include "controls/SFML/Canvas.hpp"
	#include "controls/SFML/CheckBox.hpp"
	#include "controls/SFML/ComboBox.hpp"
	#include "controls/SFML/Container.hpp"
	#include "controls/SFML/Label.hpp"
	#include "controls/SFML/ListBox.hpp"
	#include "controls/SFML/TextBox.hpp"
	#include "controls/SFML/ScrollBar.hpp"
	#include "controls/SFML/TabContainer.hpp"
	#include "controls/SFML/Terminal.hpp"
	#include "controls/SFML/TrackBar.hpp"
	#include "controls/SFML/Dialog.hpp"
	#include "controls/SFML/GridContainer.hpp"
	#include "controls/SFML/RadioButton.hpp"
	#include "controls/SFML/WidgetMover.hpp"
	#include "controls/SFML/WidgetResizer.hpp"
#endif
#include "controls/Form.hpp"
#endif
