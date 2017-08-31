#ifndef __LABELGOSSIP__PACKET__H__
#define __LABELGOSSIP__PACKET__H__

#include <set>
#include <string>

using namespace std;

namespace labelgossip {

class Packet {
 public:
	Packet(Header header) : _header(header) {}

	virtual	~Packet() {}

	virtual void get_keys(set<string>* keys) const {
		for (auto &x : _key_value) {
			keys->insert(x.first);
		}
	}

	virtual Header get_header() const {
		return _header;
	}

	virtual void add(const string& key, const string& value) {
		_key_value[key] = value;
	}

	virtual bool match(map<string, string> fix) const {
		for (auto &x : fix) {
			if (!_key_value.count(x.first) ||
			    _key_value.at(x.first) != x.second) {
				return false;
			}
		}
		return true;
	}

	virtual bool keys_match(const set<string>& keys) const {
		if (_key_value.size() != keys.size()) return false;
		for (auto &x : _key_value) {
			if (!keys.count(x.first)) return false;
		}
		return true;
	}

	virtual const map<string, string>& get() const {
		return _key_value;
	}

 protected:
	map<string, string> _key_value;
	Header _header;
};

}  // labelgossip

#endif  // __LABELGOSSIP__PACKET__H__
