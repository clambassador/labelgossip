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
		resize();
	}

	virtual void resize() {
		_size_to_format.clear();
		for (auto &x : _formats) {
			if (x.get()) {
				_size_to_format.insert(make_pair(x->size(), x.get()));
			}
		}
	}

	virtual void trace() {
		for (auto &x : _size_to_format) {
			cout << x.first << "," <<
			    x.second->to_string() << endl;
		}
	}

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
			Logger::info("data: % file %", data.length(), *x);
			Marshalled m;
			m.data(data);
			_formats.push_back(nullptr);
			_formats.back().reset(new Format());
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

        virtual const multimap<size_t, Format*>& get_formats() const {
                return _size_to_format;
        }

	virtual void merge() {
		map<Format*, set<Format*>> subset, superset;
		map<Format*, Format*> mergeset;
		size_t i = 0;

		for (auto &x : _formats) {
			for (auto &y : _formats) {
				if (x->subseteq(y.get())) {
					subset[x.get()].insert(y.get());
					superset[y.get()].insert(x.get());
				}
			}
			Logger::info("subset for % size %", ++i,
				     subset[x.get()].size());
		}
		bool exit = false;
		set<Format*> eraseset;
		while (!exit) {
			exit = true;
			for (const auto &x : _formats) {
				for (auto &y : eraseset) {
					subset[x.get()].erase(y);
				}
				eraseset.clear();
				if (subset[x.get()].size() == 2) {
					for (const auto &y : subset[x.get()]) {
						if (y == x.get()) continue;
						mergeset[x.get()] = y;
						eraseset.insert(y);
						exit = false;
					}
					subset[x.get()].erase(x.get());
				}
			}
		}
		i = 0;
		while (i < _formats.size()) {
			Format *fmt = _formats[i].get();

			if (mergeset.count(fmt)) {
				mergeset[fmt]->merge_in(fmt);
				_formats[i].reset(_formats.back().release());
				_formats.pop_back();
			} else {
				++i;
			}
		}
		resize();
	}

 protected:
	map<Header, unique_ptr<Packet>> _packets;
	vector<unique_ptr<Format>> _formats;
	multimap<size_t, Format*> _size_to_format;
};

}  // namespace labelgossip

#endif  // __LABELGOSSIP__MANAGER__H__
