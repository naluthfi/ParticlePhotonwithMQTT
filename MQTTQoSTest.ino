// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

SYSTEM_MODE(SEMI_AUTOMATIC);

boolean connectToCloud = false;

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/
byte server[] = {192,168,1,29};
MQTT client(server, 1883, callback);

// for QoS2 MQTTPUBREL message.
// this messageid maybe have store list or array structure.
uint16_t qos2messageid = 0;

unsigned long lastSend = 0;
unsigned long sendInterval = 2000;

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    String t = String(topic);
}

// QOS ack callback.
// if application use QOS1 or QOS2, MQTT server sendback ack message id.
void qoscallback(unsigned int messageid) {
    Serial.print("Ack Message Id:");
    Serial.println(messageid);

    if (messageid == qos2messageid) {
        Serial.println("Release QoS2 Message");
        client.publishRelease(qos2messageid);
    }
}

void setup() {
    Serial.begin(9600);
    
    // connect to the server
    client.connect("Testing");

    // add qos callback. If don't add qoscallback, ACK message from MQTT server is ignored.
    client.addQosCallback(qoscallback);

    // publish/subscribe
        if (client.isConnected()) {
            // it can use messageid parameter at 4.
            uint16_t messageid;
            client.publish("outTopic", "hello world", MQTT::QOS1, &messageid);
            Serial.println(messageid);
    
            // if 4th parameter don't set or NULL, application can not check the message id to the ACK message from MQTT server. 
            client.publish("outTopic", "hello world QOS1(message is NULL)", MQTT::QOS1);
        
            // QOS=2
            client.publish("outTopic", "hello world QOS2", MQTT::QOS2, &messageid);
            Serial.println(messageid);
        
            client.subscribe("inTopic");
        }
    pinMode(D7, OUTPUT);
    attachInterrupt(D1, connect, FALLING);
    IPAddress myAddress(192,168,1,111);
    IPAddress netmask(255,255,255,0);
    IPAddress gateway(192,168,1,1);
    IPAddress dns(192,168,1,1);
    WiFi.setStaticIP(myAddress, netmask, gateway, dns);

    // now let's use the configured IP
    WiFi.useStaticIP();
    WiFi.on();
    WiFi.connect();
    
    while (WiFi.ready() == false){ ; }
    
    delay(1000);
}

void loop() {
    digitalWrite(D7, HIGH);
    delay(400);
    digitalWrite(D7, LOW);
    delay(400);

    if (client.isConnected()){
        client.loop();
        if(millis() >= lastSend+sendInterval){
            lastSend = millis();
            uint16_t messageid;
            client.publish("terus","hi", MQTT::QOS1, &messageid);
            Serial.println(messageid);
            client.publish("terus","hallo", MQTT::QOS2, &messageid);
            Serial.println(messageid);
            digitalWrite(D7, HIGH);
            delay(1000);
            digitalWrite(D7, LOW);
            delay(1000);
        }
    }
    // If MQTT client is not connected then reconnect.
    if (!client.isConnected())
    {
      client.connect("Testing");
    }
    if(connectToCloud && Particle.connected() == false) {
        //Particle.connect();
        connectToCloud = false;
    }
}

void connect() {
    connectToCloud = true;
}