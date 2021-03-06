#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/pair_finder.h"

#include <map>
#include <string>
#include <vector>

using namespace labelgossip;
using namespace std;

int main(int argc, char** argv) {
	CSVTable csv;
	PairFinder mgr;
	if (argc == 1) {
		Logger::error("usage % csvfile", argv[0]);
		return 0;
	}

	csv.stream(argv[1]);
	mgr.set_data(&csv);
	mgr.find_pairs();
	mgr.trace();
}
