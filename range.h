#ifndef __LABELGOSSIP__RANGE__H__
#define __LABELGOSSIP__RANGE__H__

#include <map>
#include <set>
#include <string>

using namespace std;

namespace labelgossip {

class Range {
 public:
	Range() {}
	virtual ~Range() {}

	virtual void grow(const map<string, string>& values) {
		for (const auto &x : values) {
			_key_to_values[x.first].insert(x.second);
		}
	}

	virtual const set<string>& range(const string& key) const {
		Logger::info("range  % %", key, _key_to_values);
		if (!_key_to_values.count(key)) return _null;
		return _key_to_values.at(key);
	}

	virtual bool match(const string& key, const string& value) const {
		if (_key_to_values.size() == 0) return true;
		if (_key_to_values.count(key) == 0) return true;
		if (_key_to_values.at(key).count(value) != 0) return true;
		return false;
	}

 protected:
	map<string, set<string>> _key_to_values;
	set<string> _null;
};

}  // labelgossip

#endif  // __LABELGOSSIP__RANGE__H__
