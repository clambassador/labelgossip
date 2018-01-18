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
		        if (it->first < 4) break;
			if (!it->second->printable()) continue;
			string name = Logger::stringify(
			    "% (% packets)", i++, it->first);
			_fmts[name] = it->second;
			_sorted_formats.push_back(make_pair(name, it->second));
			Logger::info("fmt % => %", i - 1, it->second->name());
		}
	}

	virtual ~LabelgossipWeb() {}

	virtual void get_page(const ClientID& cid, int state,
	                      string* output) {
                BasePropertyPage bpp;
		bpp.set("cid", cid);
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
		if (name == "grep") {
			string grep = arguments.at("val");
			set_grep(cid, grep);
			return true;
		}
		if (name == "grep_keys") {
			string grep = arguments.at("val");
			set_grep_keys(cid, grep);
			return true;
		}
		if (name == "format") {
			size_t number = atoi(arguments.at("val").c_str());

			if (number < _cid_to_format_list[cid].size()) {
				string format_name =
				    _cid_to_format_list[cid][number];
				_cid_to_format[cid] = _fmts[format_name];
				_cid_to_narrow[cid]->clear();
				_cid_to_range[cid].reset(new Range());
				_cid_to_format[cid]->get_range(
				    _cid_to_range[cid].get(),
				    *_cid_to_narrow[cid].get());
			} else {
				_cid_to_format[cid] = nullptr;
			}
			return true;
		}
		if (name == "narrow") {
			if (parameters.size() < 2) return false;
			string key = parameters[0];
			string value = parameters[1];
			if (value == "*") {
				_cid_to_narrow[cid]->unnarrow(key);
			} else {
				_cid_to_narrow[cid]->narrow(key, value);
			}
			_cid_to_range[cid].reset(new Range());
			_cid_to_format[cid]->get_range(
				    _cid_to_range[cid].get(),
				    *_cid_to_narrow[cid].get());
		}
		return false;
	}

	virtual bool get_value(const ClientID& cid, int state,
			       const string& name,
			       const vector<string>& parameters,
			       const map<string, string>& arguments,
			       string* output) {
		if (name == "what_else") {
			stringstream ss;
			string arg = arguments.at("val");
			set <string> names;
			for (auto &x : _fmts) {
				x.second->keys_for_val(arg, &names);
			}
			for (auto &x : names) {
				ss << x << ", ";
			}
			*output = ss.str();
			return true;
		}
		if (name == "opts") {
			stringstream ss;
			for (const auto &x : _cid_to_format_list.at(cid)) {
				ss << x << endl;
			}
			*output = ss.str();
			return true;
		}
		if (!_cid_to_format[cid]) return false;
		if (name == "dests") {
			stringstream ss;
			set<string> dests;
			_cid_to_format[cid]->filter_dests(
				*_cid_to_narrow[cid].get(), &dests);
			for (const auto &x : dests) {
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
			return true;
		}
		if (name == "narrowed") {
			stringstream ss;
			*output = _cid_to_narrow[cid]->get(arguments.at("key"));
			return true;
		}
		if (name == "range") {
			stringstream ss;
			const auto &range =
			    _cid_to_range[cid]->range(arguments.at("key"));
			for (const auto &x : range) {
				ss << x << endl;
			}
			*output = ss.str();
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
		_cid_to_narrow[cid].reset(new Range());
		set_grep(cid, "");
	}

	virtual void bye_client(const ClientID& cid) {
		cleanup(cid);
	}

protected:
	virtual void set_grep(const ClientID& cid, const string& grep) {
		_cid_to_format_list[cid].clear();
		for (auto &x : _sorted_formats) {
			if (x.second->matches_dest(grep)) {
				_cid_to_format_list[cid].push_back(x.first);
			}
		}
	}

	virtual void set_grep_keys(const ClientID& cid, const string& grep) {
		_cid_to_format_list[cid].clear();
		for (auto &x : _sorted_formats) {
			if (x.second->matches_keys(grep)) {
				_cid_to_format_list[cid].push_back(x.first);
			}
		}
	}

	virtual void cleanup(const ClientID& cid) {
		_cid_to_format.erase(cid);
		_cid_to_range.erase(cid);
		_cid_to_narrow.erase(cid);
	}

	map<ClientID, Format*> _cid_to_format;
	map<ClientID, vector<string>> _cid_to_format_list;
	map<ClientID, unique_ptr<Range>> _cid_to_narrow;
	map<ClientID, unique_ptr<Range>> _cid_to_range;
	map<string, Format*> _fmts;
	vector<pair<string, Format*>> _sorted_formats;

	unique_ptr<ScaffoldNode> _node;
};

}  // namespace minibus

#endif  // __LABELGOSSIP__WEB__LABELGOSSIP_WEB__H__
