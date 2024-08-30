/* json11
 *
 * json11 is a tiny JSON library for C++11, providing JSON parsing and serialization.
 *
 * The core object provided by the library is json11::Json. A Json object represents any JSON
 * value: null, bool, number (int or double), string (string), array (vector), or
 * object (map).
 *
 * Json objects act like values: they can be assigned, copied, moved, compared for equality or
 * order, etc. There are also helper methods Json::dump, to serialize a Json to a string, and
 * Json::parse (static) to parse a string as a Json object.
 *
 * Internally, the various types of Json object are represented by the JsonValue class
 * hierarchy.
 *
 * A note on numbers - JSON specifies the syntax of number formatting but not its semantics,
 * so some JSON implementations distinguish between integers and floating-point numbers, while
 * some don't. In json11, we choose the latter. Because some JSON implementations (namely
 * Javascript itself) treat all numbers as the same type, distinguishing the two leads
 * to JSON that will be *silently* changed by a round-trip through those implementations.
 * Dangerous! To avoid that risk, json11 stores all numbers as double internally, but also
 * provides integer helpers.
 *
 * Fortunately, double-precision IEEE754 ('double') can precisely store any integer in the
 * range +/-2^53, which includes every 'int' on most systems. (Timestamps often use int64
 * or long long to avoid the Y2038K problem; a double storing microseconds since some epoch
 * will be exact for +/- 275 years.)
 */

/* Copyright (c) 2013 Dropbox, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#include "support/configCosmos.h"

//#include <string>
//#include <vector>
//#include <map>
//#include <memory>
//#include <initializer_list>

#ifdef _MSC_VER
    #if _MSC_VER <= 1800 // VS 2013
        #ifndef noexcept
            #define noexcept throw()
        #endif

        #ifndef snprintf
            #define snprintf _snprintf_s
        #endif
    #endif
#endif

namespace json11 {

enum JsonParse {
    STANDARD, COMMENTS
};

class JsonValue;

class Json final {
public:
    // Types
    enum Type {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    // Array and object typedefs
    typedef vector<Json> array;
    typedef map<string, Json> object;

    // Constructors for the various types of JSON value.
    Json() noexcept;                // NUL
    Json(std::nullptr_t) noexcept;  // NUL
    Json(double value);             // NUMBER
    Json(int value);                // NUMBER
    Json(long value);                // NUMBER
//    Json(int32_t value);
//    Json(uint32_t value);
//    Json(uint16_t value);
//    Json(uint8_t value);
    Json(bool value);               // BOOL
    Json(const string &value); // STRING
    Json(string &&value);      // STRING
    Json(const char * value);       // STRING
    Json(const array &values);      // ARRAY
    Json(array &&values);           // ARRAY
    Json(const object &values);     // OBJECT
    Json(object &&values);          // OBJECT

    // Implicit constructor: anything with a to_json() function.
    template <class T, class = decltype(&T::to_json)>
    Json(const T & t) : Json(t.to_json()) {}

    // Implicit constructor: map-like objects (map, std::unordered_map, etc)
    template <class M, typename std::enable_if<
        std::is_constructible<string, decltype(std::declval<M>().begin()->first)>::value
        && std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
            int>::type = 0>
    Json(const M & m) : Json(object(m.begin(), m.end())) {}

    // Implicit constructor: vector-like objects (std::list, vector, std::set, etc)
    template <class V, typename std::enable_if<
        std::is_constructible<Json, decltype(*std::declval<V>().begin())>::value,
            int>::type = 0>
    Json(const V & v) : Json(array(v.begin(), v.end())) {}

    // This prevents Json(some_pointer) from accidentally producing a bool. Use
    // Json(bool(some_pointer)) if that behavior is desired.
    Json(void *) = delete;

    // Accessors
    Type type() const;
    string type_name() const;

    bool is_null()   const { return type() == NUL; }
    bool is_number() const { return type() == NUMBER; }
    bool is_bool()   const { return type() == BOOL; }
    bool is_string() const { return type() == STRING; }
    bool is_array()  const { return type() == ARRAY; }
    bool is_object() const { return type() == OBJECT; }

    // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
    // distinguish between integer and non-integer numbers - number_value(), int_value(), and long_value()
    // can all be applied to a NUMBER-typed object.
    double number_value() const;
    int int_value() const;
    long long_value() const;

    // Return the enclosed value if this is a boolean, false otherwise.
    bool bool_value() const;
    // Return the enclosed string if this is a string, "" otherwise.
    const string &string_value() const;
    // Return the enclosed vector if this is an array, or an empty vector otherwise.
    const array &array_items() const;
    // Return the enclosed map if this is an object, or an empty map otherwise.
    const object &object_items() const;

    // Return a reference to arr[i] if this is an array, Json() otherwise.
    const Json & operator[](size_t i) const;
    // Return a reference to obj[key] if this is an object, Json() otherwise.
    const Json & operator[](const string &key) const;

    // Serialize.
    void dump(string &out) const;
    string dump() const {
        string out;
        dump(out);
        return out;
    }

    // Parse. If parse fails, return Json() and assign an error message to err.
    static Json parse(const string & in,
                      string & err,
                      JsonParse strategy = JsonParse::STANDARD);
    static Json parse(const char * in,
                      string & err,
                      JsonParse strategy = JsonParse::STANDARD) {
        if (in) {
            return parse(string(in), err, strategy);
        } else {
            err = "null input";
            return nullptr;
        }
    }
    // Parse multiple objects, concatenated or separated by whitespace
    static vector<Json> parse_multi(
        const string & in,
        string::size_type & parser_stop_pos,
        string & err,
        JsonParse strategy = JsonParse::STANDARD);

    static inline vector<Json> parse_multi(
        const string & in,
        string & err,
        JsonParse strategy = JsonParse::STANDARD) {
        string::size_type parser_stop_pos;
        return parse_multi(in, parser_stop_pos, err, strategy);
    }

    bool operator== (const Json &rhs) const;
    bool operator<  (const Json &rhs) const;
    bool operator!= (const Json &rhs) const { return !(*this == rhs); }
    bool operator<= (const Json &rhs) const { return !(rhs < *this); }
    bool operator>  (const Json &rhs) const { return  (rhs < *this); }
    bool operator>= (const Json &rhs) const { return !(*this < rhs); }

    /* has_shape(types, err)
     *
     * Return true if this is a JSON object and, for each item in types, has a field of
     * the given type. If not, return false and set err to a descriptive message.
     */
    typedef std::initializer_list<std::pair<string, Type>> shape;
    bool has_shape(const shape & types, string & err) const;

private:
    std::shared_ptr<JsonValue> m_ptr;
};

// Internal class hierarchy - JsonValue objects are not exposed to users of this API.
class JsonValue {
protected:
    friend class Json;
    friend class JsonInt;
    friend class JsonLong;
    friend class JsonDouble;
    virtual Json::Type type() const = 0;
    virtual bool equals(const JsonValue * other) const = 0;
    virtual bool less(const JsonValue * other) const = 0;
    virtual void dump(string &out) const = 0;
    virtual double number_value() const;
    virtual int int_value() const;
    virtual int long_value() const; // ERIC:  you probably meant to return long here?  I ain't touching it, though!
    virtual bool bool_value() const;
    virtual const string &string_value() const;
    virtual const Json::array &array_items() const;
    virtual const Json &operator[](size_t i) const;
    virtual const Json::object &object_items() const;
    virtual const Json &operator[](const string &key) const;
    virtual ~JsonValue() {}
};


// Additional JSON Object Interface (jsonface)

string pretty_string(const json11::Json& json, unsigned int depth = 0, unsigned int indent_spaces = 4, bool newline_simple_array_elements = false);


// Put json object to output stream
std::ostream& operator<<(std::ostream& os, const json11::Json& json);
// Get json object from input stream
std::istream& operator>>(std::istream& is, json11::Json& json);


/// Combines two JSON objects into a composite JSON object and returns the result
Json operator+(const Json& lhs, const Json& rhs);

/// Combines two JSON objects into a composite JSON object and replaces lhs
Json& operator+=(Json& lhs, const Json& rhs);

// this function might already exist in json11
bool has_json_key(const Json& json, const string& key);


// FIND JSON OBJECT BY KEY

/// populate all results found with JSON key == key_name
inline void find_json_object(const Json &json, const string &key_name, vector<Json> &json_results);


// Forward declaration of the template function
template<typename... Keys>
inline Json find_json_object(const Json& json, const std::string& firstKey, Keys... rest);

// Base case for the recursion when there's only one key left to process
inline Json find_json_object(const Json& json, const std::string& key_name) {
	if (json.is_object()) {
		for (const auto& kv : json.object_items()) {
			if (kv.first == key_name) {
				//return kv.second;  // Return the found object directly
				return Json::object { {kv.first, kv.second} };
			}

			auto result = find_json_object(kv.second, key_name);
			if (!result.is_null()) {
				return result;
			}
		}
	} else if (json.is_array()) {
		for (const auto& item : json.array_items()) {
			auto result = find_json_object(item, key_name);
			if (!result.is_null()) {
				return result;
			}
		}
	}

	// Return a null Json object if no matching key is found
	return Json();
}

// Template function to handle multiple keys
template<typename... Keys>
inline Json find_json_object(const Json& json, const std::string& firstKey, Keys... rest) {
	Json result = find_json_object(json, firstKey);
	if (!result.is_null() && sizeof...(rest) > 0) {  // If a result is found and there are more keys
		return find_json_object(result, rest...);  // Recurse with the remaining keys
	}
	return result;  // Return the final found object or a null Json if none is found
}


// FIND_JSON_VALUE_BY_KEY

// Declaration for single key
template <typename T>
inline T find_json_value(const Json& json, const string& key);

// Declaration for multiple keys (variadic template)
template <typename T, typename... Keys>
inline T find_json_value(const Json& json, const std::string& firstKey, Keys... rest);


// Specialization for int
template <>
inline int find_json_value<int>(const Json& json, const string& key) {
	if (key.empty())
		return json.int_value();
	else
		return find_json_object(json, key)[key].int_value();
}

// Specialization for double
template <>
inline double find_json_value<double>(const Json& json, const string& key) {
	if (key.empty())
		return json.number_value(); // Assuming number_value() returns double
	else
		return find_json_object(json, key)[key].number_value();
}

// Specialization for string
template <>
inline string find_json_value<string>(const Json& json, const string& key) {
	if (key.empty())
		return json.string_value();
	else
		return find_json_object(json, key)[key].string_value();
}

// Specialization for Json objects
template <>
inline Json find_json_value<Json>(const Json& json, const string& key) {
    if (key.empty()) {
        // If the key is empty, return the entire Json object
        return json;
    } else {
        // If a key is provided, find and return the Json object associated with the key
        Json foundJson = find_json_object(json, key);
        if (!foundJson[key].is_null()) {
            return foundJson[key];
        } else {
            // If the key is not found or the value is null, return a null Json object
            return Json();
        }
    }
}

// Specialization for bool
// bools are really Json objects, so just use above...


// Specialization for vector<int>
template <>
inline std::vector<int> find_json_value<std::vector<int>>(const Json& json, const string& key) {
    std::vector<int> result;
    Json array = find_json_object(json, key)[key];
    if (array.is_array()) {
        for (const auto& item : array.array_items()) {
            result.push_back(item.int_value());  // Extract int value from each item
        }
    }
    return result;
}

// Specialization for vector<double>
template <>
inline std::vector<double> find_json_value<std::vector<double>>(const Json& json, const string& key) {
    std::vector<double> result;
    Json array = find_json_object(json, key)[key];
    if (array.is_array()) {
        for (const auto& item : array.array_items()) {
            result.push_back(item.number_value());  // Extract double value from each item
        }
    }
    return result;
}

// Specialization for vector<string>
template <>
inline std::vector<string> find_json_value<std::vector<string>>(const Json& json, const string& key) {
    std::vector<string> result;
    Json array = find_json_object(json, key)[key];
    if (array.is_array()) {
        for (const auto& item : array.array_items()) {
            result.push_back(item.string_value());  // Extract string value from each item
        }
    }
    return result;
}

// Specialization for vector<bool>
template <>
inline std::vector<bool> find_json_value<std::vector<bool>>(const Json& json, const string& key) {
    std::vector<bool> result;
    Json array = find_json_object(json, key)[key];
    if (array.is_array()) {
        for (const auto& item : array.array_items()) {
            result.push_back(item.bool_value());  // Extract bool value from each item
        }
    }
    return result;
}

// Specialization for vector<Json>
template <>
inline std::vector<Json> find_json_value<std::vector<Json>>(const Json& json, const string& key) {
    std::vector<Json> result;
    Json array = find_json_object(json, key)[key];
    if (array.is_array()) {
        for (const auto& item : array.array_items()) {
            result.push_back(item);  // Directly push Json items into the result vector
        }
    }
    return result;

}

// Specialization for multiple key
template <typename T, typename... Keys>
inline T find_json_value(const Json& json, const std::string& firstKey, Keys... rest) {
	//if constexpr (sizeof...(rest) == 0) {
	if (sizeof...(rest) == 0) {
		// If there are no more keys, directly extract the value from the key-value pair object
		Json keyValueObject = find_json_object(json, firstKey);
		if (!keyValueObject.is_null()) {
			if (!keyValueObject[firstKey].is_null()) {
				// Use the existing specialized single-key version of find_json_value
				return find_json_value<T>(keyValueObject, firstKey);
			} else {
				// The key does not exist in the keyValueObject, return default value
				return T{};
			}
		}
	} else {
		// Otherwise, find the next level JSON object (which includes the key) and recurse
		Json keyValueObject = find_json_object(json, firstKey);
		if (!keyValueObject.is_null()) {
			Json nextLevel = keyValueObject[firstKey];  // Extract the value, which is the next level object
			if (!nextLevel.is_null()) {
				return find_json_value<T>(nextLevel, rest...);  // Continue with the remaining keys
			}
		}
	}
	// Return default-constructed value of T if the key is not found or the next level is null
	return T{};
}


// TODO: make this work for variadic number of key_names (which means swappin the last two args...)
inline bool update_json_value(Json& json, const string& key_name, const Json& new_value) {
	if (json.is_object()) {
		auto& items = const_cast<Json::object&>(json.object_items()); // Get a modifiable reference to the object items
		for (auto& kv : items) {
			if (kv.first == key_name) {
				kv.second = new_value; // Replace the value of the object
				return true; // Stop after replacing the first instance
			} else {
				if (update_json_value(kv.second, key_name, new_value)) {
					return true; // If a replacement occurred in a nested object, stop the recursion
				}
			}
		}
	} else if (json.is_array()) {
		for (Json& item : const_cast<Json::array&>(json.array_items())) {
			if (update_json_value(item, key_name, new_value)) {
				return true; // If a replacement occurred in an array element, stop the recursion
			}
		}
	}
	return false; // Return false if no replacement was made
}

} // namespace json11

