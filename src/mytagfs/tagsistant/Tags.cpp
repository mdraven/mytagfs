#include "Tags.h"

#include <algorithm>

namespace tagsistant {

    Tags::Tags() {}

    void Tags::add(const std::string& name, int id) {
        names.push_back(name);
        ids.push_back(id);
    }

    const std::vector<std::string>& Tags::getNames() const {
        return names;
    }

    const std::vector<int>& Tags::getIds() const {
        return ids;
    }

    int Tags::getId(const std::string& tag_name) const {
        auto it = std::find(names.begin(), names.end(), tag_name);
        if(it == names.end())
            throw std::runtime_error("Unknown tagname");

        return ids[std::distance(names.begin(), it)];
    }

    void Tags::clear() {
        names.clear();
        ids.clear();
    }

    Tags::~Tags() {}

}
