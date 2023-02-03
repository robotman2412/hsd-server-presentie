
#pragma once

#include <string>

namespace Topic {

// Swiped cards are sent here.
const std::string cardSwipes("card-swipes");
// Matches found for swiped cards are sent here.
const std::string cardMatches("card-matches");
// Not found for swiped cards are sent here.
const std::string cardUnknowns("card-unknowns");

};

#include <nlohmann/json.hpp>
#include <mqtt/async_client.h>
using json = nlohmann::json;

// Called by MQTT client when connection is established.
void onMqttConnected(const std::string &reason);
// Called by MQTT client when connection is lost (but no explicit disconnect packet).
void onMqttConnectionLost(const std::string &reason);
// Called by MQTT client when message arrives.
void onMqttMessage(mqtt::const_message_ptr message);
