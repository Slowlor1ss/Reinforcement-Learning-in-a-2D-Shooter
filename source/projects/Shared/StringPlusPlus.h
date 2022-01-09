//Made together with Ruben Versavel

#pragma once
//#include <iostream>
#include <sstream>
#include <vector>

struct StringPP : public std::string
{    
    //default string constructors
    StringPP() : std::string() {}
    StringPP(const std::string& s) : std::string(s) {}
    StringPP(const char* s) : std::string(s) {}
    //StringPP(const StringPP& s) : std::string(s) {}

    StringPP Reverse() const;
    StringPP Indent(int level, char delimiter = '\t') const; //0 --> //    0
    StringPP PrefixFill(const int desiredLength, const char fillChar = '0') const;
    StringPP SuffixFill(const int desiredLength, const char fillChar = '0') const;
    StringPP ToLower() const;
    StringPP RemoveSpaces() const;
    StringPP RemoveFromText(const std::string& wordOrSomething) const;
    StringPP AddBetweenLetters(const std::string& inBetween) const;
    StringPP SurroundLetters(const std::string& surround) const;
	

    bool StartsWith(const StringPP& prefix) const;
 
    std::vector<StringPP> Split(char delim = ' ') const;

    template<typename T>
    static StringPP Join(std::vector<T> v, StringPP delimiter = " ")
    {
	    std::ostringstream vts;
        if (v.empty()) return vts.str();

        // Convert all but the last element to avoid a trailing "," 
        std::copy(v.begin(), v.end() - 1, std::ostream_iterator<std::string>(vts, delimiter.c_str()));

        // Now add the last element with no delimiter 
        vts << v.back();

        return vts.str();
    }
};
