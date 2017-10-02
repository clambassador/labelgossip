#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/labelgossip_web.h"
#include "labelgossip/manager.h"

#include "centipede/types.h"
#include "centipede/webserver.h"
#include "centipede/backend/i_webserver_backend.h"
#include "centipede/nodes/scaffold_node.h"

#include <map>
#include <string>
#include <vector>

using namespace labelgossip;
using namespace std;

int main(int argc, char** argv) {
	Config::_()->load("labelgossip.cfg");

	Manager mgr;
	mgr.load("formats");
	mgr.merge();
	mgr.save("formats_merged");
}
