#ifndef TAGS_H
#define TAGS_H

#include <vector>
#include <string>

namespace tagsistant {

    class Tags {
        std::vector<std::string> names;
        std::vector<int> ids;
    public:
        Tags();

        void add(const std::string& name, int id);

        const std::vector<std::string>& getNames() const;
        const std::vector<int>& getIds() const;
        int getId(const std::string& tag_name) const;

        void clear();

        virtual ~Tags();
    };

}

#endif // TAGS_H
