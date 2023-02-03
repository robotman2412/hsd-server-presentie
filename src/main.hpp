
#pragma once

#include "json.hpp"

#include <string>
#include <mqtt/async_client.h>

extern mqtt::async_client *client;

namespace Topic {

// Swiped cards are received here.
const std::string cardSwipes("card-swipes");
// Actions and inquiries not involving tags are received here.
const std::string inquiries("inquiries");

// Matches found for swiped cards are sent here.
const std::string cardMatches("card-matches");
// Not found for swiped cards are sent here.
const std::string cardUnknowns("card-unknowns");
// Presence changes for users are sent here.
const std::string presence("presence");

};

// Called by MQTT client when connection is established.
void onMqttConnected(const std::string &reason);
// Called by MQTT client when connection is lost (but no explicit disconnect packet).
void onMqttConnectionLost(const std::string &reason);
// Called by MQTT client when message arrives.
void onMqttMessage(mqtt::const_message_ptr message);
