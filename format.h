#ifndef __LABELGOSSIP__FORMAT__H__
#define __LABELGOSSIP__FORMAT__H__

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
		for (auto &x : _packets) {
			range->grow(x->get());
		}
	}

	virtual bool matches(const Packet& packet) const {
		return (packet.keys_match(_keys));
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

	virtual const set<string> keys() const {
		return _keys;
	}

	virtual string to_string() const {
		return Logger::stringify("%,%", _dests, _keys);
	}

 protected:
	set<string> _keys;
	set<string> _dests;
	vector<Packet*> _packets;

};

}  // labelgossip

#endif  // __LABELGOSSIP__FORMAT__H__
