#ifndef __LABELGOSSIP__ID_HISTORY__H__
#define __LABELGOSSIP__ID_HISTORY__H__

#include <string>

#include "ib/marshalled.h"

using namespace std;
using namespace ib;

namespace labelgossip {

class IdHistory {
 public:
	IdHistory(const string& value) : _value(value) {}

	virtual	~IdHistory() {}

	virtual void consider(const Header& header,
			      const string& key) {
		_apps.insert(header._app);
		_devices.insert(header._hwid);
		_names.insert(key);
	}

	virtual int get_apps() const {
		return _apps.size();
	}

	virtual void trace() const {
		Logger::info("value %", _value);
		Logger::info("   apps %", _apps.size());
		Logger::info("   devices %", _devices.size());
		Logger::info("   AKA %", _names);
		Logger::info("");
		Logger::info("");

	}
/*
	virtual void marshal(Marshalled* m) const {
		m->push(_app);
		m->push(_version);
		m->push(_hwid);
		m->push(_dir);
		m->push(_dest);
		m->push(_tls);
		m->push(_digest);
	}

	virtual void demarshal(Marshalled* m) {
		m->pull(&_app);
		m->pull(&_version);
		m->pull(&_hwid);
		m->pull(&_dir);
		m->pull(&_dest);
		m->pull(&_tls);
		m->pull(&_digest);
	}
*/
 protected:
	set<string> _apps;
	set<string> _names;
	set<string> _devices;
	string _value;
};

}  // labelgossip

#endif  // __LABELGOSSIP__ID_HISTORY__H__
