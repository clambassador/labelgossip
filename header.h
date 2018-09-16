#ifndef __LABELGOSSIP__HEADER__H__
#define __LABELGOSSIP__HEADER__H__

#include <string>

#include "ib/marshalled.h"

using namespace std;
using namespace ib;

namespace labelgossip {

class Header {
 public:
	Header() {}
	Header(const string& app, const string& version,
	       const string& hwid, const string& dir, const string& dest,
	       const string& tls, const string& digest)
		: _app(app), _version(version), _hwid(hwid),
		  _dir(dir), _dest(dest), _tls(tls), _digest(digest) {}

	virtual	~Header() {}

	virtual bool operator<(const Header& other) const {
		if (_app != other._app) return _app < other._app;
		if (_version != other._version) return _version < other._version;
		if (_hwid != other._hwid) return _hwid < other._hwid;
		if (_dir != other._dir) return _dir < other._dir;
		if (_dest != other._dest) return _dest < other._dest;
		if (_tls != other._tls) return _tls < other._tls;
		if (_digest != other._digest) return _digest < other._digest;
		return false;
	}

	virtual string to_string() const {
		return _app + "," + _dest;
	}

	virtual string dest() const {
		return _dest;
	}

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

	string _app;
	string _version;
	string _hwid;
	string _dir;
	string _dest;
	string _tls;
	string _digest;
};

}  // labelgossip

#endif  // __LABELGOSSIP__HEADER__H__
