
#include "api.hpp"
#include "main.hpp"
#include "data.hpp"

// List of user IDs for present users.
static std::vector<UserId> presence;



// ==== helper functions ==== //

// Is user currently present?
bool isPresent(const User &user) {
	return std::find(presence.begin(), presence.end(), user.id) != presence.end();
}

// Handle enter/exit actions.
void actionExtryOrExit(User &user) {
	// Look up user ID in presence list.
	auto iter = std::find(presence.begin(), presence.end(), user.id);
	
	if (iter == presence.end()) {
		// User will enter.
		presence.push_back(user.id);
		
		json obj;
		obj["user"] = user.toJson();
		obj["user"]["presence"] = true;
		client->publish(Topic::presence, obj.dump());
		
	} else {
		// User will leave.
		presence.erase(iter);
		
		json obj;
		obj["user"] = user.toJson();
		obj["user"]["presence"] = false;
		client->publish(Topic::presence, obj.dump());
	}
}

// Handle register tag actions.
void actionRegisterTag(const Tag &tag) {
	
}

// Query list of users actions.
// This can get expensive for large amounts of users.
void actionQueryUsers() {
	json obj;
	
	// Dump all users from the database into the json object.
	auto &arr = obj["users"] = json::array();
	for (auto &iter: Database::getUsers()) {
		json userJson = iter.second.toJson();
		userJson["presence"] = isPresent(iter.second);
		arr.push_back(std::move(userJson));
	}
}



// ==== API callbacks ==== //

// Handle actions triggered by card swipes.
void Api::cardSwiped(mqtt::const_message_ptr message) {
	// Card swiped: Look up the tag.
	json obj  = json::parse(message->get_payload());
	Tag  tag  = Tag::fromHexString(obj["tag"]);
	User user = User::find(tag);
	
	if (obj["action"] == "register_tag") {
		// Register this tag to the user.
		actionRegisterTag(tag);
		return;
	}
	
	if (!user) {
		// User not found.
		client->publish(Topic::cardUnknowns, "{\"tag\":\"" + tag.toHexString() + "\"}");
		return;
	}
	
	// User found.
	client->publish(Topic::cardMatches, user.toJson().dump());
	
	// Check for additional actions.
	if (obj["action"] == "enter_or_exit") {
		// Entry or exit toggle.
		actionExtryOrExit(user);
	}
}

// Handle inquiry actions.
void Api::inquiries(mqtt::const_message_ptr message) {
	// General inquiry.
	json obj  = json::parse(message->get_payload());
	
	if (obj["action"] == "query_users") {
		// Query list of all users.
		actionQueryUsers();
	}
}
