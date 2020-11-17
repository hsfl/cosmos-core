#include "support/jsondef.h"

void replace(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return;
}

vector<size_t> find_newlines(const string& sample) {
    vector<size_t> characterLocations;
    for(size_t i =0; i < sample.size(); i++) if(sample[i] == '\n') characterLocations.push_back(i);
    return characterLocations;
}

void pretty_form(string& js)    {

    replace(js, ", ", ",\n");
    replace(js, "{", "{\n");
    replace(js, "[", "[\n");
    replace(js, "}", "\n}");
    replace(js, "]", "\n]");

    // create vector[char position] = # of indents
    int indent = 0;
    vector<size_t> indents;
    for(size_t i = 0; i < js.size(); ++i)   {
        if(js[i]=='['){ ++indent;}
        if(js[i]=='{'){ ++indent;}
        if(js[i]==']'){ --indent; indents[i-1]--;}
        if(js[i]=='}'){ --indent; indents[i-1]--;}
        indents.push_back(indent);
    }

    // find position of all '\n' characters
    vector<size_t> newlines = find_newlines(js);

    // insert the appropriate # of indents after the '\n' char
    for(size_t i = newlines.size(); i!=0; ) {
		--i;
        string indent_string;
        for(size_t j = 0; j < indents[newlines[i]]; ++j)    indent_string += "  ";
        js.insert(newlines[i]+1, indent_string);
    }
    return;
}
