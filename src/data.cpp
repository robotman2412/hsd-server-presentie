
#include "data.hpp"

// Tag to user ID map.
std::map<Tag, uint64_t> tagToUser;
// User ID to user map.
std::map<uint64_t, User> users;



// Print a hexadecimal representation of `tag` to `os`.
template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const Tag &tag) {
	// This is the alphabet used to turn binary into hex.
	static const char hexstr[] = "0123456789ABCDEF";
	
	// Try to print the first character.
	if (tag.binaryId.length()) {
		// Character to print is the first.
		char c = tag.binaryId[0];
		// Print 4 MSB first, then 4 LSB.
		os << hexstr[c>>4] << hexstr[c&15];
	}
	
	// Print any extra characters.
	for (auto iter = tag.binaryId.begin()+1; iter != tag.binaryId.end(); iter++) {
		// Character to print is from iterator.
		char c = *iter;
		// Print 4 MSB first, then 4 LSB.
		os << '-' << hexstr[c>>4] << hexstr[c&15];
	}
	
	// Return `os`, required to chain `<<` calls.
	return os;
}



// Find a user from a tag.
const User User::find(const Tag &tag) {
	// This is in essence a more complicated map lookup.
	auto iter = tagToUser.find(tag);
	// Return the element if found, or a nullptr if not.
	return (iter != tagToUser.end()) ? users[iter->second] : User();
}

// Save this user, associating `tags` with this user in the database.
void User::save() const {
	if (!valid()) throw std::invalid_argument("This user is an invalid user.");
	
	// Check whether the user exists.
	auto iter = users.find(id);
	
	if (iter == users.end()) {
		// Store to id to user map.
		users[id] = *this;
		
		// Store to tag to user map.
		for (Tag tag: tags) {
			tagToUser[tag] = id;
		}
		
	} else {
		// If it does exist, update the tags.
		for (const Tag &tag: iter->second.tags) {
			tagToUser.erase(tag);
		}
		
		// Store to id to user map.
		iter->second = *this;
		
		// Store to tag to user map.
		for (Tag tag: tags) {
			tagToUser[tag] = id;
		}
	}
}



// Initialise and load database from disk.
void Database::load() {
	
}

// Store database to disk.
void Database::save() {
	
}
