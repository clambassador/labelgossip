#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/manager.h"

#include <map>
#include <string>
#include <vector>

using namespace labelgossip;
using namespace std;

int main(int argc, char** argv) {
	CSVTable csv;
	Manager mgr;
	if (argc == 1) {
		Logger::error("usage % csvfile", argv[0]);
		return 0;
	}

	csv.load(argv[1]);
	mgr.set_data(csv);
	mgr.find_formats();
	mgr.trace();
}
