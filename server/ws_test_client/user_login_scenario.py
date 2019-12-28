import websocket
import ssl
import json
import sys
import signal


quit = False


def signal_handler(sig, frame):
    global quit
    quit = True


signal.signal(signal.SIGINT, signal_handler)
ws = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
ws.connect("wss://localhost:8080/")
#ws.connect("wss://www.realmofaesir.com/")
ws.send("{\"type\": \"Auth:register\", \"username\": \"oipo\", \"password\": \"testtest\", \"email\": \"test@test.nl\"}")
res = ws.recv()
print(f'res: {res}')
res_msg = json.loads(res)
if res_msg['type'] == 'error_response':
    ws.send("{\"type\": \"Auth:login\", \"username\": \"oipo\", \"password\": \"testtest\"}")
    res = ws.recv()
    print(f'res: {res}')
    res_msg = json.loads(res)
    if res_msg['type'] == 'error_response':
        sys.exit(1)


ws.send("{\"type\": \"Game:create_character\", \"slot\": 0, \"name\": \"Oipo\", \"gender\": \"test\", \"race\": \"Pirates\", \"baseclass\": \"Mage\"}")
res = ws.recv()
print(f'res: {res}')
ws.send("{\"type\": \"Game:play_character\", \"slot\": 0}")
res = ws.recv()
print(f'res: {res}')
res_msg = json.loads(res)
if res_msg['type'] == 'error_response':
    sys.exit(1)

ws.send("{\"type\": \"Moderator:motd\", \"motd\": \"Welcome to rair!\"}")
ws.send("{\"type\": \"Game:move\", \"x\": 12, \"y\": 12}")
ws.send("{\"type\": \"Chat:send\", \"content\": \"chat message!\"}")
while not quit:
    res = ws.recv()
    print(f'res: {res}')
ws.close()
