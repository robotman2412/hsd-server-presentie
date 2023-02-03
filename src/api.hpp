
#pragma once

#include <string>
#include <mqtt/async_client.h>

namespace Api {

// Handle message from Topics::cardSwipes.
void cardSwiped(mqtt::const_message_ptr raw);

};
