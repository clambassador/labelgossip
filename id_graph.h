#ifndef __LABELGOSSIP__ID_GRAPH__H__
#define __LABELGOSSIP__ID_GRAPH__H__

#include <map>
#include <string>
#include <vector>

#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/header.h"
#include "labelgossip/id_history.h"

namespace labelgossip {

class IdGraph {
 public:
	IdGraph() {}

	virtual ~IdGraph() {}

	virtual void set_data(CSVTable* csv) {
		vector<string> data;
		while (csv->get_next_row(&data)) {
			if (!_value.count(data[8])) {
				_value[data[8]].reset(new IdHistory(data[8]));
			}
			_value[data[8]]->consider(Header(data[0], data[1], data[2],
				      data[3], data[4], data[5], data[6]),
						 data[7]);
		}
	}

	virtual void trace() {
		int thresh = 4;
		for (auto& x : _value) {
			int apps = x.second->get_apps();
			if (apps > thresh) {
				x.second->trace();
			}
		}
	}

	/*
	virtual void save(const string& dir) {
		int i = 0;
		for (auto &x : _size_to_format) {
			ofstream fout(
				Logger::stringify("%/fmt_%", dir, i++));
			Marshalled m(*x.second);
			fout << m.str();
		}
	}

	virtual void load(const string& dir) {
		vector<string> files;
		Fileutil::list_directory(dir, &files);
		for (auto x = files.rbegin(); x != files.rend(); ++x) {
			if (x->substr(0, 4) != "fmt_") continue;
			string data;
			string filename = Logger::stringify("%/%", dir, *x);
			Fileutil::read_file(filename, &data);
			Marshalled m;
			m.data(data);
			_formats.push_back(nullptr);
			_formats.back().reset(new Format(filename));
			m.pull(_formats.back().get());

			const vector<Packet*>& packets =
			    _formats.back()->get_packets();
			_size_to_format.insert(make_pair(
				packets.size(), _formats.back().get()));
			for (const auto& y : packets) {
				_packets[y->get_header()].reset(y);
			}
		}
	}
*/

 protected:
	map<string, unique_ptr<IdHistory>> _value;
};

}  // namespace labelgossip

#endif  // __LABELGOSSIP__ID_GRAPH__H__
