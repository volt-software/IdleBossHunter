import websocket
import ssl
import json
import sys

ws = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
ws.connect("wss://localhost:8080/")
#ws.connect("wss://62.210.141.213:8081/")
ws.send("{\"type\": \"Auth:register\", \"username\": \"char_sel\", \"password\": \"testtest\", \"email\": \"test@test.nl\"}")
res = ws.recv()
print(f'res: {res}')
res_msg = json.loads(res)
if res_msg['type'] == 'error_response':
    ws.send("{\"type\": \"Auth:login\", \"username\": \"char_sel\", \"password\": \"testtest\"}")
    res = ws.recv()
    print(f'res: {res}')
    res_msg = json.loads(res)
    if res_msg['type'] == 'error_response':
        sys.exit(1)


ws.send("{\"type\": \"Game:character_select\" }")

res = ws.recv()
print(f'res: {res}')
