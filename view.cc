#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/manager.h"

#include <map>
#include <string>
#include <vector>

using namespace labelgossip;
using namespace std;

int main(int argc, char** argv) {
	Manager mgr;
	mgr.load("formats");
	mgr.save("check");
	mgr.trace();
}
