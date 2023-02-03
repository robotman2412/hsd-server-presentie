
#pragma once

#include "json.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

struct Tag {
	// This string stores bytes that represent the tag's ID.
	std::string binaryId;
	
	// Default tag: empty, invalid.
	inline Tag() = default;
	// Set from string.
	inline Tag(const std::string &in) : binaryId(in) {}
	// Set from binary array.
	inline Tag(const char *in, size_t len) : binaryId(in, len) {}
	// Set from hex encoded string.
	// Example input: 01-23-45-67-89-ab-cd-ef (case insensitive).
	// Allowed (optional) separators: space , . -
	static inline Tag fromHexString(const std::string &in);
	// Turn this into a hex string.
	// Example output: 01-23-45-67-89-ab-cd-ef
	inline std::string toHexString() const;
	
	// Is this tag valid?
	operator bool() const noexcept { return binaryId.length(); }
	// Is this tag valid?
	inline bool valid() const noexcept { return binaryId.length(); }
	
	// Comparisons operators.
	inline bool operator==(const Tag &other) const noexcept { return binaryId == other.binaryId; }
	// Comparisons operators.
	inline bool operator!=(const Tag &other) const noexcept { return binaryId != other.binaryId; }
	// Comparisons operators.
	inline bool operator< (const Tag &other) const noexcept { return binaryId <  other.binaryId; }
	// Comparisons operators.
	inline bool operator> (const Tag &other) const noexcept { return binaryId >  other.binaryId; }
	// Comparisons operators.
	inline bool operator<=(const Tag &other) const noexcept { return binaryId <= other.binaryId; }
	// Comparisons operators.
	inline bool operator>=(const Tag &other) const noexcept { return binaryId >= other.binaryId; }
};

// Print a hexadecimal representation of `tag` to `os`.
// Example output: 01-23-45-67-89-ab-cd-ef
template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const Tag &tag);

// Read `tag` from hexadecimal representation in `is`.
// Example input: 01-23-45-67-89-ab-cd-ef (case insensitive).
// Allowed (optional) separators: space , . -
template<typename CharT, typename Traits>
std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, Tag &tag);

// Set from hex encoded string.
Tag Tag::fromHexString(const std::string &str) {
	Tag out;
	std::stringstream in(str);
	in >> out;
	if (!in.eof()) return Tag();
	return out;
}

// Turn this into a hex string.
std::string Tag::toHexString() const {
	std::stringstream os;
	os << *this;
	return os.str();
}

typedef uint64_t UserId;

struct User {
	// User is invalid if this ID is zero.
	UserId id;
	// Username.
	std::string name;
	// User's website or web presence, if any.
	std::string web;
	// NFC tags that this user owns.
	std::vector<Tag> tags;
	
	// Create a user from a json object.
	// Returns an invalid user if `obj["id"] == 0` or if it is invalid.
	static User fromJson(const json &obj);
	// Create a json object from this user.
	json toJson() const;
	
	// Is this user valid?
	operator bool() const noexcept { return id != 0; }
	// Is this user valid?
	inline bool valid() const noexcept { return id != 0; }
	
	// Find a user from a tag.
	static User find(const Tag &tag);
	// Save this user, associating `tags` with this user in the database.
	void save() const;
};

namespace Database {

// Initialise and load database from disk.
void load();
// Store database to disk.
void save();
// Get a const reference to the users map.
const std::map<UserId, User> &getUsers();

}

