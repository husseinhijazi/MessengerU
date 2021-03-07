import socket
from _thread import *
import SqlLite
import Client
import Message
import datetime
import uuid


class Server:
    # Definition of all Variables and Constants
    __SQL: SqlLite.SqlLite = None
    __ERROR: str = '9000'
    __VER: str = '2'
    __HOST: str = ''
    __PORT: int = -1
    __BUFSIZE: int = 0
    REGREQ = b'100'
    USRSREQ = b'101'
    PBKREQ = b'102'
    SMREQ = b'103'
    GWMREQ = b'104'
    REGRES = 1000
    USRSRES = 1001
    PBKRES = 1002
    SMRES = 1003
    GWMRES = 1004
    CENDSTR = '\0'
    PROTOCOLSEPERATION = '/'
    PROTOCOLINNERSEPERATION = ','

    # constructor
    def __init__(self, port: int, host: str, sql: SqlLite.SqlLite, bufsize: int):

        self.__SQL = sql
        self.__HOST = host
        self.__PORT = port
        self.__BUFSIZE = bufsize

    # PROTOCOL request handling
    def __serve(self, addr: tuple, conn: socket.socket):
        ver = self.__VER
        code = self.__ERROR
        payLoad = ''
        paySize = len(payLoad)
        sendBuf = str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(paySize) + self.PROTOCOLSEPERATION + payLoad + self.CENDSTR
        sendBuf = sendBuf.encode()
        try:
            print('connected to ' + str(addr))
            while True:
                data = conn.recv(self.__BUFSIZE)
                if len(data) > 5:
                    reqCode = self.__getRequestCode(data)

                    if reqCode == self.REGREQ:

                        sendBuf = self.__register(data)

                    elif reqCode == self.USRSREQ:

                        self.__updateLSeen(data)
                        sendBuf = self.__getUsers(data)

                    elif reqCode == self.PBKREQ:

                        self.__updateLSeen(data)
                        sendBuf = self.__getPublicKeyFromDB(data)

                    elif reqCode == self.SMREQ:

                        self.__updateLSeen(data)
                        sendBuf = self.__getsTheMsg(data)

                    elif reqCode == self.GWMREQ:

                        self.__updateLSeen(data)
                        sendBuf = self.__getWaitingMsgs(data)
                    break

        except socket.error as e:
            print(e)
        conn.sendall(sendBuf)
        conn.close()

    # main method to run the server it's make a socket and handling with multi connections
    def runServer(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.bind((self.__HOST, self.__PORT))
            sock.listen(5)
            while True:
                conn, addr = sock.accept()
                start_new_thread(self.__serve, (addr, conn,))
            sock.close()

    # made a new UUID and adding a new client to DB by SQL object
    def __addClient(self, name, pkey, ls):
        cli = Client.Client(uuid.uuid4().hex, name, pkey, ls)
        return self.__SQL.insertCli(cli)

    # adding a new msg to DB by SQL object
    def __addMsg(self, toc, frc, typ, content):
        msg = Message.Message(toc, frc, typ, content)
        return self.__SQL.insertMsg(msg)

    # handling the register request
    def __register(self, data:bytes):
        ver = ''
        try:
            ver = self.__VER
            lseen = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")
            name = self.__getName(data)
            pKey = self.__getPublicKey(data)
            clientID = self.__addClient(name.decode(), pKey.decode(), lseen)

            code = self.REGRES
            if clientID == -1:
                code = self.__ERROR
                clientID = ''
        except:
            clientID = ''
            code = self.__ERROR

        sendBuf = str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(len(str(clientID))) + self.PROTOCOLSEPERATION + str(clientID) + self.CENDSTR
        return sendBuf.encode()

    # handling the get clients request
    def __getUsers(self, data: bytes):
        ver = self.__VER
        code = self.__ERROR
        payL = ''
        try:
            cliId = str(self.__getClientId(data).decode())
            if len(str(self.__SQL.getCliByID(cliId))) > 2:
                code = self.USRSRES
                allCli = self.__SQL.getAllCli()
                for cli in allCli:
                    if str(cli[0]) != cliId:
                        payL += str(cli[0])
                        payL += self.PROTOCOLINNERSEPERATION
                        payL += str(cli[1])
                        payL += self.PROTOCOLINNERSEPERATION
                payL = payL[:len(payL) - 1]

        except:
            code = self.__ERROR
        sendBuf = str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(len(payL)) + self.PROTOCOLSEPERATION + str(payL) + self.CENDSTR
        return sendBuf.encode()

    # handling the get public key request
    def __getPublicKeyFromDB(self, data: bytes):
        ver = ''
        try:
            ver = self.__VER
            code = self.__ERROR
            payL = ''
            cliName = str(self.__getPayLoad(data).decode())
            cli = self.__getClientId(data)
            if len(str(self.__SQL.getCliByID(str(cli.decode())))) > 2:
                cli = self.__SQL.getPKeyCliByName(cliName[:len(cliName) - 1])
                cli = cli[0]
                payL = str(cli[0]) + self.PROTOCOLINNERSEPERATION + str(cli[1])
                code = self.PBKRES

        except:
            payL = ''
            code = self.__ERROR

        sendBuf = str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(len(payL)) + self.PROTOCOLSEPERATION + str(payL)
        return sendBuf.encode()

    # handling the send msg request
    def __getsTheMsg(self, data:bytes):
        ver = self.__VER
        code = self.__ERROR
        payL = ''
        try:
            frC = str(self.__getClientId(data).decode())
            data = self.__getPayLoad(data)
            data = data.split(self.PROTOCOLINNERSEPERATION.encode())
            toC = str(data[0].decode())
            if len(str(self.__SQL.getCliByID(frC))) > 2 and len(str(self.__SQL.getCliByID(toC))) > 2:
                code = self.SMRES
                mC = data[3]
                i = 4
                while i < len(data):
                    mC += self.PROTOCOLINNERSEPERATION.encode() + data[i]
                    i += 1

                msgId = self.__addMsg(toC, frC, str(data[1].decode()), mC[:len(mC) - 1])
                payL = str(toC) + self.PROTOCOLSEPERATION + str(msgId)
        except:
            payL = ''
            code = self.__ERROR
        sendBuf = str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(len(payL)) + self.PROTOCOLSEPERATION + payL + self.CENDSTR
        return sendBuf.encode()

    # handling the get msgs request
    def __getWaitingMsgs(self, data:bytes):
        ver = self.__VER
        code = self.GWMRES
        payL = ''.encode()
        try:
            cliID = str(self.__getClientId(data).decode())

            allMsgs = self.__SQL.getMsgByToClId(cliID)
            for msg in allMsgs:
                payL += 'start'.encode()
                payL += str(msg[0]).encode()
                payL += ','.encode()
                payL += str(msg[1]).encode()
                payL += ','.encode()
                payL += str(msg[2]).encode()
                payL += ','.encode()
                payL += str(len(msg[3])).encode()
                payL += ','.encode()
                payL += msg[3]
                payL += '\0end'.encode()

            self.__SQL.deleteMsgByToClId(cliID)
        except:
            payL = ''.encode()
            code = self.__ERROR

        sendBuf = (str(ver) + self.PROTOCOLSEPERATION + str(code) + self.PROTOCOLSEPERATION + str(len(payL)) + self.PROTOCOLSEPERATION).encode() + payL + self.CENDSTR.encode()
        return sendBuf

    # getting the client id from request data
    def __getClientId(self, data:bytes):
        try:

            req = data.split(self.PROTOCOLSEPERATION.encode())
            return req[0]
        except:
            print('Cli Id e')

        return None

    # getting the version from request data
    def __getVersion(self, data:bytes):
        try:

            req = data.split(self.PROTOCOLSEPERATION.encode())
            return req[1]
        except:
            print('Ver e1')

        return None

    # getting the request code from request data
    def __getRequestCode(self, data: bytes):
        try:

            req = data.split(self.PROTOCOLSEPERATION.encode())
            return req[2]
        except:
            print('Code e')

        return None

    # getting the pay load size from request data
    def __getPaySize(self, data: bytes):
        try:

            req = data.split(self.PROTOCOLSEPERATION.encode())
            return req[3]
        except:
            print('ERROR paySize')

        return None

    # getting the pay load from request data
    def __getPayLoad(self, data: bytes):

        try:
            splits = data.split(self.PROTOCOLSEPERATION.encode())
            payL = splits[4]

            i = 5
            while i < len(splits):
                payL += self.PROTOCOLSEPERATION.encode() + splits[i]
                i += 1
            return payL
        except:
            print('ERROR PAYLOAD')
        return None

    # getting the public key from request data
    def __getPublicKey(self, data: bytes):
        try:
            pKey = self.__getPayLoad(data).split(self.PROTOCOLINNERSEPERATION.encode())

            return pKey[1]
        except:
            print('public Key error')
        return None

    # getting the name from request data
    def __getName(self, data: bytes):
        try:
            name = self.__getPayLoad(data).split(self.PROTOCOLINNERSEPERATION.encode())

            return name[0]
        except:
            print('name error')

        return None

    # handling the update of last seen by SQL object
    def __updateLSeen(self, data: bytes):
        cliId = self.__getClientId(data)
        lseen = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")
        self.__SQL.updateDateTime(str(cliId.decode()), lseen)
