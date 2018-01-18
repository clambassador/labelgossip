#ifndef __LABELGOSSIP__PAIR_FINDER__H__
#define __LABELGOSSIP__PAIR_FINDER__H__

#include <map>
#include <string>
#include <vector>

#include "ib/csv_table.h"
#include "ib/logger.h"

namespace labelgossip {

class PairFinder {
 public:
	PairFinder() {}

	virtual ~PairFinder() {}

	virtual void set_data(CSVTable* csv) {
		vector<string> data;
		while (csv->get_next_row(&data)) {
			_packet_to_keys[data[0]].insert(data[1]);
			data.clear();
		}
	}

	virtual void find_pairs() {
		for (auto &x : _packet_to_keys) {
			for (auto &y : x.second) {
				_key_to_count[y]++;
				for (auto &z : x.second) {
					if (z == y) continue;
					_pairs_to_count[make_pair(y, z)]++;
				}
			}
		}
	}

	virtual void trace() {
		for (auto &x : _pairs_to_count) {
			uint64_t percent = x.second * 100 /
				_key_to_count[x.first.first];
			cout << x.first.first << "," << x.first.second
			     << "," << x.second << "," <<
			     _key_to_count[x.first.first] << ","<< percent << endl;
		}
	}

 protected:
	map<string, set<string> > _packet_to_keys;
	map<string, uint64_t> _key_to_count;
	map<pair<string, string>, uint64_t> _pairs_to_count;
};

}  // namespace labelgossip

#endif  // __LABELGOSSIP__PAIR_FINDER__H__
