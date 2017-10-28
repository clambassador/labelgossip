#ifndef __LABELGOSSIP__FORMAT__H__
#define __LABELGOSSIP__FORMAT__H__

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "labelgossip/header.h"
#include "labelgossip/range.h"
#include "labelgossip/packet.h"

using namespace std;

namespace labelgossip {

class Format {
 public:
	Format() {}
	Format(const string& filename) : _filename(filename) {}
	virtual ~Format() {}

	virtual string name() const {
		return _filename;
	}

	virtual void keys_for_val(string val, set<string>* keys) const {
		for (const auto &x : _packets) {
			x->keys_for_val(val, keys);
		}
	}

	virtual void get_range(Range* range) {
		get_range(range, Range());
	}

	virtual void get_range(Range* range, const Range& fixed) {
		for (auto &x : _packets) {
			if (x->match(fixed)) {
				range->grow(x->get());
			} else {
			}
		}
	}

	virtual bool matches(const Packet& packet) const {
		return (packet.keys_match(_keys));
	}

	virtual bool matches_keys(const string& key) const {
		if (key.empty()) return true;
		for (auto &x: _keys) {
			auto it = search(x.begin(), x.end(),
                                         key.begin(), key.end(),
                                         [](char ch1, char ch2) {
                                                return toupper(ch1) ==
                                                       toupper(ch2);
                                        });
                        if (it != x.end()) return true;
		}
		return false;
	}

	virtual bool matches_dest(const string& dest) const {
		if (dest.empty()) return true;
		for (const auto &x : _dests) {
			auto it = search(x.begin(), x.end(),
					 dest.begin(), dest.end(),
					 [](char ch1, char ch2) {
						return toupper(ch1) ==
						       toupper(ch2);
					});
			if (it != x.end()) return true;
		}
		return false;
	}

	virtual void add(Packet* packet) {
		if (_keys.empty()) {
			packet->get_keys(&_keys);
		}
		_packets.push_back(packet);
		_dests.insert(packet->get_header().dest());
	}

	virtual size_t size() const {
		return _packets.size();
	}

	virtual const set<string> dests() const {
		return _dests;
	}

	virtual void filter_dests(const Range& fixed, set<string>* dests) const {
		for (auto &x : _packets) {
			if (x->match(fixed)) {
				dests->insert(x->get_header().dest());
			}
		}
	}

	virtual const set<string> keys() const {
		return _keys;
	}

	virtual string to_string() const {
		return Logger::stringify("%,%", _dests, _keys);
	}

	virtual void demarshal(Marshalled *m) {
		m->pull(&_keys);
		m->pull(&_dests);
		m->pull(&_packets);
	}

	virtual void marshal(Marshalled *m) const {
		m->push(_keys);
		m->push(_dests);
		m->push(_packets);
	}

	virtual const vector<Packet*>& get_packets() const {
		return _packets;
	}

	virtual bool subseteq(Format* other) const {
		if (_keys.size() > other->_keys.size()) {
			return false;
		}
		for (const auto& x : _keys) {
			if (!other->_keys.count(x)) {
				return false;
			}
		}
		return true;
	}

	virtual void merge_in(Format* other) {
		assert(other->subseteq(this));
		bool dest_match = false;
		for (const auto& x : other->_dests) {
			if (_dests.count(x)) dest_match = true;
			_dests.insert(x);
		}
		for (const auto& x : other->_packets) {
			_packets.push_back(x);
			x->set_defaults(_keys);
		}
		Logger::info("Merged in % for % (matching dest %)",
			     other->_packets.size(),
			     _keys, dest_match);
	}

 protected:
	set<string> _keys;
	set<string> _dests;
	vector<Packet*> _packets;
	string _filename;
};

}  // labelgossip

#endif  // __LABELGOSSIP__FORMAT__H__
