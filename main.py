# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.


import Server
import SqlLite

PORTFILENAME = 'port.info'
SERVERFILENAME = 'server.db'
HOST = '127.0.0.1'
PORT = -1
BUFSIZE = 4096


def getPort():
    try:
        with open(PORTFILENAME,'r') as file:
            data = file.read()
            if data.isdigit():
                data = int(data)
                if (data > 0) and (data < 65536):
                    return data
                return -1
    except FileExistsError as exc:
        print(str(exc))
    return -1

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    try:
        PORT = getPort()
        if PORT > 0:
            sqlDb = SqlLite.SqlLite(SERVERFILENAME)
            s = Server.Server(PORT,HOST,sqlDb,BUFSIZE)
            s.runServer()

    except Exception as e:
        print(e)
        print('there was an error while running the server')

    else:
        print('there was an error while the Main Function')
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
