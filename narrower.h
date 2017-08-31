#ifndef __NARROWER__H__
#define __NARROWER__H__

#include <set>

using namespace std;

namespace labelgossip {

class Narrower {
public
	Narrower() {}
	virtual ~Narrower() {}

protected:
	virtual void set_data(CSVTable* table) {
		_table = table
	}

	virtual void set_narrow_field(size_t col, const string& val) {
		good();
		check_col(col);
		check_val(col, val);
		_narrow[col] = val;
		rebuild();
	}

	virtual bool good() {
		return true;  // for concurrency later
	}

	virtual void rebuild() {

	}

	map<size_t, string> _narrow;
};


}  // namespace labelgossip

#endif  // __NARROWER__H__
