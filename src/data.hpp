
#pragma once

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>

struct Tag {
	// This string stores bytes that represent the tag's ID.
	const std::string binaryId;
	
	// Set from string.
	inline Tag(const std::string &in) : binaryId(in) {}
	// Set from binary array.
	inline Tag(const char *in, size_t len) : binaryId(in, len) {}
	
	// Comparisons operators.
	inline bool operator==(const Tag &other) const { return binaryId == other.binaryId; }
	// Comparisons operators.
	inline bool operator!=(const Tag &other) const { return binaryId != other.binaryId; }
	// Comparisons operators.
	inline bool operator< (const Tag &other) const { return binaryId <  other.binaryId; }
	// Comparisons operators.
	inline bool operator> (const Tag &other) const { return binaryId >  other.binaryId; }
	// Comparisons operators.
	inline bool operator<=(const Tag &other) const { return binaryId <= other.binaryId; }
	// Comparisons operators.
	inline bool operator>=(const Tag &other) const { return binaryId >= other.binaryId; }
	
	// Turn this into a hex string.
	inline std::string toHexString() const;
};

// Print a hexadecimal representation of `tag` to `os`.
template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const Tag &tag);

// Turn this into a hex string.
std::string Tag::toHexString() const {
	std::stringstream os;
	os << *this;
	return os.str();
}

struct User {
	// User is invalid if this ID is zero.
	uint64_t id;
	// Username.
	std::string name;
	// User's local hackerspace.
	// Space homeSpace;
	// User's website or web presence, if any.
	std::string web;
	// NFC tags that this user owns.
	std::vector<Tag> tags;
	
	// Is this user valid?
	operator bool() const { return id != 0; }
	// Is this user valid?
	inline bool valid() const { return id != 0; }
	
	// Find a user from a tag.
	static const User find(const Tag &tag);
	// Save this user, associating `tags` with this user in the database.
	void save() const;
};

namespace Database {

// Initialise and load database from disk.
void load();
// Store database to disk.
void save();

}

