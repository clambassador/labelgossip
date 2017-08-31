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
 protected:
	map<string, set<string>> _key_to_values;
};

}  // labelgossip

#endif  // __LABELGOSSIP__RANGE__H__
