#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/id_graph.h"

#include <map>
#include <string>
#include <vector>

using namespace labelgossip;
using namespace std;

int main(int argc, char** argv) {
	CSVTable csv;
	IdGraph idg;
	if (argc == 1) {
		Logger::error("usage % csvfile", argv[0]);
		return 0;
	}

	csv.stream(argv[1]);
	idg.set_data(&csv);
	idg.trace();
}
