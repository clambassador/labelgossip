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
	virtual ~Format() {}

	virtual void get_range(Range* range) {
		get_range(range, Range());
	}

	virtual void get_range(Range* range, const Range& fixed) {
		for (auto &x : _packets) {
			if (x->match(fixed)) {
				range->grow(x->get());
			} else {
				assert(0); // todo for now
			}
		}
	}

	virtual bool matches(const Packet& packet) const {
		return (packet.keys_match(_keys));
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

 protected:
	set<string> _keys;
	set<string> _dests;
	vector<Packet*> _packets;

};

}  // labelgossip

#endif  // __LABELGOSSIP__FORMAT__H__
