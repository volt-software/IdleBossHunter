import websocket
import ssl
import json
import sys
import threading
import signal


quit = False


def signal_handler(sig, frame):
    global quit
    quit = True


def get_message_type(res):
    res_msg = json.loads(res)
    return res_msg['type']


def wait_for_these_types(ws, i, types):
    global quit
    while not quit:
        res = ws.recv()
        print(f'{i} res: {res}')
        msg_type = get_message_type(res)
        if msg_type in types:
            return msg_type

def run_thread(i):
    global quit
    ws = websocket.WebSocket(sslopt={"cert_reqs": ssl.CERT_NONE})
    ws.settimeout(5)
    ws.connect("ws://localhost:8080/")
    #ws.connect("wss://www.realmofaesir.com/")
    ws.send("{\"type\": \"Auth:register\", \"username\": \"many_user" + str(i) + "\", \"password\": \"testtest\", \"email\": \"test@test.nl\"}")
    msg_type = wait_for_these_types(ws, i, ['error_response', 'Auth:login_response'])

    if msg_type == 'error_response':
        ws.send("{\"type\": \"Auth:login\", \"username\": \"many_user" + str(i) + "\", \"password\": \"testtest\"}")
        msg_type = wait_for_these_types(ws, i, ['error_response', 'Auth:login_response'])

    if msg_type == 'error_response':
        ws.close()
        return

    ws.send("{\"type\": \"Game:create_character\", \"slot\": 0, \"name\": \"MU" + ('a'*i) + "\", \"gender\": \"test\", \"race\": \"Pirates\", \"baseclass\": \"Mage\"}")
    msg_type = wait_for_these_types(ws, i, ['error_response', 'Game:create_character_response'])
    ws.send("{\"type\": \"Game:play_character\", \"slot\": 0}")

    msg_type = wait_for_these_types(ws, i, ['error_response', 'Game:map_update'])

    if msg_type == 'error_response':
        ws.close()
        return

    ws.send("{\"type\": \"Game:move\", \"x\": 12, \"y\": 12}")
    ws.send("{\"type\": \"Chat:send\", \"content\": \"chat message!\"}")
    try:
        while not quit:
            res = ws.recv()
            print(f'{i} res: {res}')
    except:
        pass
    ws.close()


signal.signal(signal.SIGINT, signal_handler)
threads = []
for i in range(20):
    x = threading.Thread(target=run_thread, args=(i,))
    x.start()
    threads.append(x)

for i in range(20):
    threads[i].join()
