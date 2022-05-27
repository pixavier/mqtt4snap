
import threading
import paho.mqtt.client as mqtt
from datetime import datetime

semaphore = 0
resp = ''

topic = 'name'
responseTopic = 'name2/callback'
payload = 'Hello'

def on_message(clientId, userdata, message):
    global semaphore, resp
    print('on_message')
    resp = message.payload.decode("utf-8")
    print(message.topic)
    print(resp)
    semaphore = 1


mqttc = mqtt.Client('idClient-' + datetime.now().strftime("%H:%M:%S.%f"))
mqttc.on_message = on_message
mqttc.connect("broker.emqx.io", 1883)
mqttc.subscribe(responseTopic)
mqttc.loop_start() 

mqttc.publish(topic + '/@' + responseTopic, str(payload))	

while semaphore == 0:
    pass

print ('Done')


