
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
	if (tag.binaryId.length() > 1) {
		for (auto iter = tag.binaryId.begin()+1; iter != tag.binaryId.end(); iter++) {
			// Character to print is from iterator.
			char c = *iter;
			// Print 4 MSB first, then 4 LSB.
			os << '-' << hexstr[c>>4] << hexstr[c&15];
		}
	}
	
	// Return `os`, required to chain `<<` calls.
	return os;
}

// 1-2 character hexadecimal to binary helper, returns -1 on error.
template<typename CharT, typename Traits>
static inline int unhexHelper(std::basic_istream<CharT, Traits> &is) {
	int out, a, b;
	a = is.get();
	
	if (a >= '0' && a <= '9') {
		out = a - '0';
		
	} else if (a >= 'a' && a <= 'f') {
		out = a - 'a' + 0x0a;
		
	} else if (a >= 'A' && a <= 'F') {
		out = a - 'A' + 0x0a;
		
	} else {
		is.seekg((int) is.tellg() - 1);
		return -1;
	}
	
	b = is.get();
	if (b >= '0' && b <= '9') {
		out <<= 4; out |= b - '0';
		
	} else if (b >= 'a' && b <= 'f') {
		out <<= 4; out |= b - 'a' + 0x0a;
		
	} else if (b >= 'A' && b <= 'F') {
		out <<= 4; out |= b - 'A' + 0x0a;
		
	} else if (b >= 0) {
		is.seekg((int) is.tellg() - 2);
		return -1;
	}
	
	return out;
}


// Read `tag` from hexadecimal representation in `is`.
// Example input: 01-23-45-67-89-ab-cd-ef (case insensitive).
// Allowed (optional) separators: space , . -
template<typename CharT, typename Traits>
std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, Tag &tag) {
	// Get first one in the thing.
	int match = unhexHelper(is);
	
	// If fail, return an empty one.
	tag = Tag();
	if (match < 0) return is;
	
	// Check for separator char type.
	int sep = is.get();
	if (sep != ' ' && sep != ',' && sep != '.' && sep != '-') {
		sep = 0;
	}
	is.seekg((int) is.tellg() - 1);
	
	// Otherwise, start making a tag.
	while (match >= 0) {
		tag.binaryId.push_back(match);
		if (sep && sep != is.get()) return is;
		match = unhexHelper(is);
	}
	
	return is;
}



// Create a user from a json object.
// Returns an invalid user if `obj["id"] == 0` or if it is invalid.
User User::fromJson(const json &in) {
	try {
		User out;
		
		// Always present: `id` and `name`.
		out.id = in["id"];
		out.name = in["name"];
		// Optional: `web`.
		if (in.contains("web")) {
			out.web = in["web"];
		}
		// Array of tags.
		for (std::string raw: in["tags"]) {
			// Get tag from string representation.
			Tag tag = Tag::fromHexString(raw);
			// On success, add as normal.
			if (tag) out.tags.push_back(std::move(tag));
			// On fail, abort and return empty user.
			else return User();
		}
		
		return out;
		
	} catch(...) {
		// Any exception? Return an empty user.
		return User();
	}
}

// Create a json object from this user.
json User::toJson() const {
	json out;
	
	// Always present: `id` and `name`.
	out["id"]   = id;
	out["name"] = name;
	// Optional: `web`.
	if (web.length()) out["web"] = web;
	// Encode list of tags.
	auto &arr = out["tags"] = std::vector<std::string>();
	for (Tag t: tags) {
		arr.push_back(t.toHexString());
	}
	
	return out;
}


// Find a user from a tag.
User User::find(const Tag &tag) {
	// This is in essence a more complicated map lookup.
	auto iter = tagToUser.find(tag);
	// Return the element if found, or a nullptr if not.
	return (iter != tagToUser.end()) ? users[iter->second] : User();
}

// Save user to database helper.
static void saveUserHelper(const User &user) {
	// Store to id to user map.
	users[user.id] = user;
	
	// Store to tag to user map.
	for (Tag tag: user.tags) {
		// Check if it already exists in the map.
		auto iter = tagToUser.find(tag);
		if (iter == tagToUser.end()) {
			// If not, simply assign it.
			tagToUser[tag] = user.id;
			
		} else {
			// If it does, remove from old user.
			auto &prev = users[iter->second];
			prev.tags.erase(std::find(prev.tags.begin(), prev.tags.end(), iter->first));
			std::cout << "Reassigning tag " << tag << " from " << prev.id << " to " << user.id << '\n';
			
			// Before reassigning to new user.
			iter->second = user.id;
		}
	}
}

// Save this user, associating `tags` with this user in the database.
void User::save() const {
	if (!valid()) throw std::invalid_argument("This user is an invalid user.");
	
	// Check whether the user exists.
	auto iter = users.find(id);
	
	if (iter == users.end()) {
		saveUserHelper(*this);
	} else {
		// If it does exist, update the tags.
		for (const Tag &tag: iter->second.tags) {
			tagToUser.erase(tag);
		}
		saveUserHelper(*this);
	}
}



// Initialise and load database from disk.
void Database::load() {
	// Read and parse the `users.json` file.
	std::ifstream in("data/users.json");
	json obj;
	try {
		in >> obj;
	} catch(json::parse_error err) {
		std::cout << "JSON parse error at position " << err.byte << ": " << err.what() << '\n';
	}
	
	// Double-check that parsed is an array.
	if (!obj.is_array()) {
		std::cout << "Error: `users.json` is not an array.\n";
	}
	
	// It is iterating time.
	for (auto raw: obj) {
		// Try to parse user from json.
		User user = User::fromJson(raw);
		
		// Discard failed users.
		if (!user) {
			std::cout << "Invalid user object " << raw << "\n";
			continue;
		}
		
		// Discard uplicate users.
		if (users.find(user.id) != users.end()) {
			std::cout << "Discarding duplicate user with id " << user.id << "\n";
			continue;
		}
		
		// Store user to the lookup tables.
		saveUserHelper(user);
	}
}

// Store database to disk.
void Database::save() {
	json obj = json::array();
	
	// Iterate over users map.
	for (auto &iter: users) {
		// Find a valid user.
		auto &user = iter.second;
		if (!user) continue;
		
		// Turn it into json and store it.
		obj.push_back(user.toJson());
	}
	
	// Open the `users.json` file and save the object.
	std::ofstream out("data/users.json");
	out << obj;
}
