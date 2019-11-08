import websocket
import ssl
import json
import sys

ws = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
#ws.connect("wss://localhost:8080/")
ws.connect("wss://62.210.141.213:8080/")
ws.send("{\"type\": \"Auth:register\", \"username\": \"cat\", \"password\": \"testtest\", \"email\": \"test@test.nl\"}")
res = ws.recv()
print(f'res: {res}')
res_msg = json.loads(res)
if res_msg['type'] == 'error_response':
    ws.send("{\"type\": \"Auth:login\", \"username\": \"cat\", \"password\": \"testtest\"}")
    res = ws.recv()
    print(f'res: {res}')
    res_msg = json.loads(res)
    if res_msg['type'] == 'error_response':
        sys.exit(1)


ws.send("{\"type\": \"Game:create_character\", \"slot\": 0, \"name\": \"Cat\", \"gender\": \"test\", \"race\": \"Pirates\", \"baseclass\": \"Mage\"}")
res = ws.recv()
print(f'res: {res}')
ws.send("{\"type\": \"Game:play_character\", \"slot\": 0}")
res = ws.recv()
print(f'res: {res}')
res_msg = json.loads(res)
if res_msg['type'] == 'error_response':
    sys.exit(1)

ws.send("{\"type\": \"Game:move\", \"x\": 12, \"y\": 12}")
ws.send("{\"type\": \"Game:move\", \"x\": 14, \"y\": 14}")
ws.send("{\"type\": \"Chat:send\", \"content\": \"chat message!\"}")
while 1:
    res = ws.recv()
    print(f'res: {res}')
