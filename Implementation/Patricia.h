#include <string>
#include <list>

bool isEmpty(std::string s) {
    return s.empty();
}

struct simpleStruct {
    std::list<std::string> strings;
    void update((std::list<std::string> a) m(std::list<std::string> b));
    std::string split();
}

void update((std::list<std::string> a) m(std::list<std::string> b)) {
    strings = m(strings);
    strings.remove_if(isEmpty);
}

std::string split() {
    int size = strings.size();
    std::string ret = "";
    int count = 0;
    int success = 1;
    char tmp = strings.front[0];
    for(int c = 0; c < 255; c++) {
        for(int i = 0; i < size; i++) {
            if(strings[i][count] != '\0' && strings[i][count] != tmp) {
                success = 0;
                break;
            }
        }

        if(!success) {
            break;
        }

        count++;
        tmp = strings.front[count];
    }

    count--;
    if(count < 0) {
        return std::string("");
    }
    strings[0].copy(ret, count, 0);
    for(int i = 0; i < size; i++) {
        strings[i].erase(0, count);
    }
    strings.remove_if(isEmpty);
    return ret;
}
