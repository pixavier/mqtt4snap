import threading
import paho.mqtt.client as mqtt
from datetime import datetime

mqttHost = 'broker.emqx.io'
mqttPort = 1883

ts = datetime.timestamp(datetime.now())
mqttc = mqtt.Client('idClient-' + str(ts))
#mqttc.username_pw_set(username="user", password="pass")
mqttc.connect(mqttHost, mqttPort)

def on_message(clientId, userdata, message):
    try:
        payload = message.payload.decode("utf-8", errors='ignore')
        fulltopic = message.topic
        if fulltopic.index('/idCall-') > 0:
            topic = fulltopic[:fulltopic.rindex('/')]
            callbackTopic = fulltopic[fulltopic.rindex('/')+1:]
            mqttc.publish(callbackTopic, 'Echo from MQTT 3.1 bridge: ' + payload)	
            
            print('------------------------------')
            print('full topic: ' + fulltopic)
            print('topic:' + topic)
            print('payload: ' + payload)
            print('callbackTopic:' + callbackTopic)
  
    except Exception as e:
#        print(e)
        pass
    

mqttc.on_message = on_message
mqttc.subscribe('topicName/#')
print ("MQTT 3.1 bridge running ...")
mqttc.loop_forever()
