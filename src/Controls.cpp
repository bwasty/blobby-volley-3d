
#include "Controls.h"

using namespace VRS;
using namespace BV3D;

Controls::Controls() {
	for(int i=0;i<5;i++) m_Command[i] = false;
}

Controls::~Controls() {
}

bool Controls::isRequested(COMMAND cmd) {
	return m_Command[cmd];
}
