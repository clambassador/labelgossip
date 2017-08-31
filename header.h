#ifndef __LABELGOSSIP__HEADER__H__
#define __LABELGOSSIP__HEADER__H__

#include <string>

using namespace std;

namespace labelgossip {

class Header {
 public:
	Header() {}
	Header(const string& app, const string& version,
	       const string& hwid, const string& dest,
	       const string& tls, const string& digest)
		: _app(app), _version(version), _hwid(hwid),
		  _dest(dest), _tls(tls), _digest(digest) {}

	virtual	~Header() {}

	virtual bool operator<(const Header& other) const {
		if (_app != other._app) return _app < other._app;
		if (_version != other._version) return _version < other._version;
		if (_hwid != other._hwid) return _hwid < other._hwid;
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

 protected:
	string _app;
	string _version;
	string _hwid;
	string _dest;
	string _tls;
	string _digest;
};

}  // labelgossip

#endif  // __LABELGOSSIP__HEADER__H__
