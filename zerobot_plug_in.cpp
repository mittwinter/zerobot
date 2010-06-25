#include "zerobot_plug_in.hpp"

namespace zerobot {

PlugIn::PlugIn(unsigned int _priority, std::string const &_name) : priority(_priority), name(_name) {
}

}
