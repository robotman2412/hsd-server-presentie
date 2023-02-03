
#include "api.hpp"
#include "main.hpp"
#include "data.hpp"

// List of user IDs for present users.
static std::vector<UserId> presence;



// ==== helper functions ==== //

// Handle entry/exit actions.
void actionExtryOrExit(User &user) {
	// Look up user ID in presence list.
	auto iter = std::find(presence.begin(), presence.end(), user.id);
	
	if (iter == presence.end()) {
		// User will enter.
		presence.push_back(user.id);
		
		json obj;
		obj["user"] = user.toJson();
		obj["presence"] = true;
		client->publish(Topic::presence, obj.dump());
		
	} else {
		// User will leave.
		presence.erase(iter);
		
		json obj;
		obj["user"] = user.toJson();
		obj["presence"] = false;
		client->publish(Topic::presence, obj.dump());
	}
}



// ==== API callbacks ==== //

// Handle message from Topics::cardSwipes.
void Api::cardSwiped(mqtt::const_message_ptr message) {
	// Card swiped: Look up the tag.
	json obj  = json::parse(message->get_payload());
	Tag  tag  = Tag::fromHexString(obj["tag"]);
	User user = User::find(tag);
	
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
