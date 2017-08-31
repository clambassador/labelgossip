#ifndef __LABELGOSSIP__MANAGER__H__
#define __LABELGOSSIP__MANAGER__H__

#include <map>
#include <string>
#include <vector>

#include "ib/csv_table.h"
#include "ib/logger.h"
#include "labelgossip/format.h"
#include "labelgossip/header.h"
#include "labelgossip/packet.h"

namespace labelgossip {

class Manager {
 public:
	Manager() {}

	virtual ~Manager() {}

	virtual void set_data(const CSVTable& csv) {
		size_t n = csv.rows();
		for (size_t i = 0; i < n; ++i) {
			vector<string> data;
			csv.get_row(i, &data);
			Header header(data[0], data[1], data[2],
				      data[3], data[4], data[5]);
			if (!_packets.count(header)) {
				_packets[header].reset(new Packet(header));
			}
			_packets[header]->add(data[6], data[7]);
		}
	}

	virtual void find_formats() {
		for (auto& x : _packets) {
			bool found = false;
			for (auto &y: _formats) {
				if (y->matches(*x.second.get())) {
					y->add(x.second.get());
					found = true;
					break;
				}
			}
			if (!found) {
				_formats.push_back(nullptr);
				_formats.back().reset(new Format());
				_formats.back()->add(x.second.get());
			}

		}
		for (auto &x : _formats) {
			_size_to_format.insert(make_pair(x->size(), x.get()));
		}
	}

	virtual void trace() {
		for (auto &x : _size_to_format) {
			cout << x.first << "," <<
			    x.second->to_string() << endl;
		}
	}

	virtual void save() {
		int i = 0;
		for (auto &x : _size_to_format) {
			ofstream fout(
				Logger::stringify("formats/fmt_%", i++));
			// write format

			// write packets
		}
	}

 protected:
	map<Header, unique_ptr<Packet>> _packets;
	vector<unique_ptr<Format>> _formats;
	multimap<size_t, Format*> _size_to_format;
};

}  // namespace labelgossip

#endif  // __LABELGOSSIP__MANAGER__H__
