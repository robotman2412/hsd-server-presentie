
#include "main.hpp"
#include "data.hpp"
#include "api.hpp"

#include <unistd.h>

mqtt::async_client *client;

int main(int argc, char **argv) {
	Database::load();
	
	client = new mqtt::async_client("tcp://localhost:1883/", "lololol");
	client->set_connected_handler(onMqttConnected);
	client->set_connection_lost_handler(onMqttConnectionLost);
	client->set_message_callback(onMqttMessage);
	client->connect();
	
	while (1) {
		usleep(1000000);
		try {
			if (!client->is_connected()) {
				client->reconnect();
			}
		} catch(...) {}
	}
}

// Called by MQTT client when connection is established.
void onMqttConnected(const std::string &reason) {
	std::cout << "MQTT connected\n";
	client->subscribe(Topic::cardSwipes, 2);
	client->subscribe(Topic::inquiries,  2);
}

// Called by MQTT client when connection is lost (but no explicit disconnect packet).
void onMqttConnectionLost(const std::string &reason) {
	std::cout << "MQTT connection lost";
	if (reason.length()) std::cout << ": " << reason;
	std::cout << '\n';
}

// Called by MQTT client when message arrives.
void onMqttMessage(mqtt::const_message_ptr message) {
	std::cout << message->get_topic() << ": " << message->get_payload_str() << '\n';
	
	try {
		if (message->get_topic() == Topic::cardSwipes) {
			Api::cardSwiped(message);
		} else if (message->get_topic() == Topic::inquiries) {
			Api::inquiries(message);
		}
	} catch(json::parse_error err) {
		std::cout << "JSON parse error at position " << err.byte << ": " << err.what() << '\n';
	} catch(json::type_error err) {
		std::cout << "JSON invalid type: " << err.what() << '\n';
	} catch(std::exception e) {
		std::cout << e.what() << '\n';
	}
}
