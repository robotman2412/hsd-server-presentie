
#pragma once

#include <string>
#include <mqtt/async_client.h>

namespace Api {

// Handle actions triggered by card swipes.
void cardSwiped(mqtt::const_message_ptr raw);
// Handle inquiry actions.
void inquiries(mqtt::const_message_ptr raw);

};
