#ifndef __LABELGOSSIP__LABELGOSSIP_WEB__H__
#define __LABELGOSSIP__LABELGOSSIP_WEB__H__

#include <cstdlib>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "ib/config.h"
#include "ib/formatting.h"

#include "centipede/types.h"
#include "centipede/webserver.h"
#include "centipede/backend/i_webserver_backend.h"
#include "centipede/nodes/scaffold_node.h"

#include "labelgossip/format.h"
#include "labelgossip/manager.h"

using namespace centipede;
using namespace ib;
using namespace std;

namespace labelgossip {

class LabelgossipWeb : public IWebserverBackend {
public:
	LabelgossipWeb() {
		_node.reset(new ScaffoldNode(
				Config::_()->gets("html_file")));
	}

	virtual void init(const Manager& mgr) {
		const multimap<size_t, Format*>& fmts = mgr.get_formats();
		int i = 0;
		for (auto it = fmts.rbegin();
		     it != fmts.rend(); ++it) {
		        if (it->first < 10) break;
			string name = Logger::stringify(
			    "% (% apps)", i++, it->first);
			_fmts.push_back(make_pair(name, it->second));
		}
	}

	virtual ~LabelgossipWeb() {}

	virtual void get_page(const ClientID& cid, int state,
	                      string* output) {
                BasePropertyPage bpp;
		bpp.set("cid", cid);
		bpp.set("options", get_format_list());
		*output = _node.get()->display(&bpp);
	}

        virtual void get_resource(const ClientID& cid,
                                  const ResourceID& rid,
                                  const string& ject,
                                  string* output) {
        }

	virtual bool set_value(const ClientID& cid, int state,
			       const string& name,
			       const vector<string>& parameters,
			       const map<string, string>& arguments) {
		if (name == "format") {
			size_t number = atoi(arguments.at("val").c_str());
			Logger::info("cid % to use %", cid, number);

			if (number < _fmts.size()) {
				_cid_to_format[cid] = _fmts[number].second;
				_cid_to_format[cid]->get_range(_cid_to_range[cid].get());
			} else {
				_cid_to_format[cid] = nullptr;
			}
			return true;
		}
		return false;
	}

	virtual bool get_value(const ClientID& cid, int state,
			       const string& name,
			       const vector<string>& parameters,
			       const map<string, string>& arguments,
			       string* output) {
		if (!_cid_to_format[cid]) return false;
		if (name == "dests") {
			stringstream ss;
			for (const auto &x : _cid_to_format[cid]->dests()) {
				ss << x << endl;
			}
			*output = ss.str();
			return true;
		}
		if (name == "keys") {
			stringstream ss;
			for (const auto &x : _cid_to_format[cid]->keys()) {
				ss << x << endl;
			}
			*output = ss.str();
			Logger::info("%", *output);
			return true;
		}
		if (name == "range") {
			stringstream ss;
			for (const auto &x :
			     _cid_to_range[cid]->range(arguments.at("key"))) {
				ss << x << endl;
			}
			*output = ss.str();
			Logger::info("got the range for % is %",
				     arguments.at("key"),
				     _cid_to_range[cid]->range(arguments.at("key")));
			return true;
		}
		if (name == "screen") {
			return true;
		}
		if (name == "format") {
			return true;
		}
		Logger::error("unknown value %", name);
		*output = "";
		return false;
	}

	virtual int run_command(const ClientID& cid, int state,
	                        const string& command,
				const vector<string>& parameters,
				const map<string, string>& arguments) {
		return 0;
	}

        virtual void run_node_command(const ClientID&, int state,
                                      const string& node,
                                      const string& command,
                                      const vector<string>& parameters,
                                      const map<string, string>& arguments) {
	}

        virtual int recv_post(const ClientID&, const string& command,
                              const string& key, const string& filename,
                              const string& content_type, const string& encoding,
                              const string& data, uint64_t offset,
                              size_t size, string* output) {
		return 0;
	}

	virtual void new_client(const ClientID& cid) {
		_cid_to_format[cid] = nullptr;
		_cid_to_range[cid].reset(new Range());
	}

	virtual void bye_client(const ClientID& cid) {
		cleanup(cid);
	}

protected:
	virtual void cleanup(const ClientID& cid) {
		_cid_to_format.erase(cid);
		_cid_to_range.erase(cid);
	}

	virtual string get_format_list() const {
		stringstream ss;

		for (const auto& x: _fmts) {
			ss << "<option>" << x.first << "</option>"
			   << endl;
		}
		Logger::error("%", ss.str());
		return ss.str();
	}

	map<ClientID, Format*> _cid_to_format;
	map<ClientID, unique_ptr<Range>> _cid_to_range;
	vector<pair<string, Format*>> _fmts;

	unique_ptr<ScaffoldNode> _node;
};

}  // namespace minibus

#endif  // __LABELGOSSIP__WEB__LABELGOSSIP_WEB__H__
