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

	LabelgossipWeb lw;
	lw.init(mgr);
	WebServer webserver(&lw);
	webserver.start_server(Config::_()->get("http_port"));

	cout << Config::_()->get("http_port") << endl;
	cout << "Running.\nHit any key to stop.";

	getchar();
	webserver.stop_server();
}
