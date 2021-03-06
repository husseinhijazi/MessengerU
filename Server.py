

import socket
from _thread import *
import SqlLite
import Client
import Message
import datetime
import uuid


class Server:
    SQL:SqlLite.SqlLite = None
    ERROR:str = '9000'
    VER:str = '2'
    HOST:str = ''
    PORT:int = -1
    BUFSIZE:int = 0

    def __init__(self,port:int,host:str,sql:SqlLite.SqlLite,bufsize:int):

        self.SQL = sql
        self.HOST = host
        self.PORT = port
        self.BUFSIZE = bufsize

    def __serve(self, addr:tuple, conn:socket.socket):
        try:
            print('connected to ' + str(addr))
            while True:
                data = conn.recv(self.BUFSIZE)
                if len(data) > 5:

                    print(data)
                    reqCode = self.getRequestCode(data)
                    ver = self.VER
                    code = self.ERROR
                    payLoad = ''
                    paySize = payLoad.__len__()
                    sendBuf = str(ver)+'/'+str(code)+'/'+str(paySize)+'/'+payLoad+'\0'
                    sendBuf = sendBuf.encode()

                    if reqCode == b'100':

                        sendBuf = self.register(data)

                    elif reqCode == b'101':

                        self.updateLSeen(data)
                        sendBuf = self.getUsers(data)


                    elif reqCode == b'102':

                        self.updateLSeen(data)
                        sendBuf = self.getPublicKeyFromDB(data)

                    elif reqCode == b'103':

                        self.updateLSeen(data)
                        sendBuf = self.getsTheMsg(data)

                    elif reqCode == b'104':

                        self.updateLSeen(data)
                        sendBuf = self.getWaitingMsgs(data)

                    conn.sendall(sendBuf)
                    break
        except socket.error as e:
            print(e)

        conn.close()

    def runServer(self):
        with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as s:
           s.bind((self.HOST,self.PORT))
           s.listen(5)
           while True:
                conn, addr = s.accept()
                start_new_thread(self.__serve,(addr,conn,))



    def addClient(self,name,pKey,ls):
        cli = Client.Client(uuid.uuid4().hex,name,pKey,ls)
        return  self.SQL.insertCli(cli)

    def addMsg(self,toC,frC,type,content):
        msg = Message.Message(toC,frC,type,content)
        return self.SQL.insertMsg(msg)

    def register(self,Data:bytes):
        ver = ''
        try:
            ver = self.VER
            lseen = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")
            name = self.getName(Data)
            pKey = self.getPublicKey(Data)
            clientID = self.addClient(name.decode(),pKey.decode(),lseen)

            code = 1000
            if clientID == -1:
                code = self.ERROR
                clientID = ''
        except:
            clientID = ''
            code = self.ERROR

        sendBuf = str(ver) + '/' + str(code) + '/' + str(len(str(clientID))) + '/' + str(clientID) + '\0'
        return sendBuf.encode()


    def getUsers(self, Data:bytes):
        ver = self.VER
        code = self.ERROR
        payL = ''
        try:
            cliId = str(self.getClientId(Data).decode())
            if len(str(self.SQL.getCliByID(cliId))) > 2:
                code = 1001
                allCli = self.SQL.getAllCli()
                for cli in allCli:
                    if str(cli[0]) != cliId:
                        payL += str(cli[0])
                        payL += ','
                        payL += str(cli[1])
                        payL += ','
                payL = payL[:len(payL)-1]

        except :
            code = self.ERROR
        sendBuf = str(ver) + '/' + str(code) + '/' + str(len(payL)) + '/' + payL+'\0'
        return sendBuf.encode()

    def getPublicKeyFromDB(self,Data:bytes):
        ver = ''
        try:
            ver = self.VER
            code = self.ERROR
            payL = ''
            cliName = str(self.getPayLoad(Data).decode())
            cli = self.getClientId(Data)
            if len(str(self.SQL.getCliByID(str(cli.decode())))) > 2:
                cli = self.SQL.getPKeyCliByName(cliName[:len(cliName)-1])
                cli = cli[0]
                payL = str(cli[0])+','+str(cli[1])
                code = 1002

        except:
            payL = ''
            code = self.ERROR

        sendBuf = str(ver) + '/' + str(code) + '/' + \
                  str(len(payL)) + '/' + payL
        return sendBuf.encode()

    def getsTheMsg(self,Data:bytes):
        ver = self.VER
        code = self.ERROR
        payL = ''
        try:
            data = self.getPayLoad(Data)
            data = data.split(b',')

            frC = str(self.getClientId(Data).decode())
            toC = str(data[0].decode())
            if len(str(self.SQL.getCliByID(frC))) > 2 and len(str(self.SQL.getCliByID(toC))) > 2:
                code = 1003
                mC = data[3]
                i = 4
                while i < len(data):
                    mC += b','+data[i]
                    i += 1

                msgId = self.addMsg(toC,frC,str(data[1].decode()),mC[:len(mC)-1])
                payL = str(toC)+','+str(msgId)
        except:
            payL = ''
            code = self.ERROR
        sendBuf = str(ver)+'/'+str(code)+'/'+str(len(payL))+'/'+payL+'\0'
        return sendBuf.encode()

    def getWaitingMsgs(self,Data:bytes):
        ver = self.VER
        code = 1004
        payL = ''.encode()
        try:
            cliID = str(self.getClientId(Data).decode())

            allMsgs = self.SQL.getMsgByToClId(cliID)
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

            self.SQL.deleteMsgByToClId(cliID)
        except:
            payL = ''.encode()
            code=self.ERROR

        sendBuf = str(ver).encode() + '/'.encode() + str(code).encode() +\
                  '/'.encode() + str(len(payL)).encode() + '/'.encode()\
                  + payL + '\0'.encode()


        return sendBuf

    def getClientId(self, Data: bytes):
        try:

            req = Data.split(b'/')
            return req[0]
        except:
            print('Cli Id e')

        return None

    def getVersion(self, Data:bytes):
        try:

            req = Data.split(b'/')
            return req[1]
        except:
            print('Ver e1')

        return None

    def getRequestCode(self,Data:bytes):
        try:

            req=Data.split(b'/')
            return req[2]
        except :
            print('Code e')

        return None


    def getPaySize(self, Data: bytes):
        try:

            req = Data.split(b'/')
            return req[3]
        except:
            print('ERROR paySize')

        return None


    def getPayLoad(self, Data: bytes):

        try:
            splits = Data.split(b'/')
            payL = splits[4]

            i = 5
            while i < len(splits):
                payL += b'/' + splits[i]
                i += 1
            return payL
        except:
            print('ERROR PAYLOAD')
        return None


    def getPublicKey(self,Data:bytes):
        try:
            pKey = self.getPayLoad(Data).split(b',')

            return pKey[1]
        except:
            print('public Key error')
        return None

    def getName(self,Data:bytes):
        try:
            name = self.getPayLoad(Data).split(b',')

            return name[0]
        except:
            print('name error')

        return None

    def updateLSeen(self,data:bytes):
        cliId = self.getClientId(data)
        lseen = datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S")
        self.SQL.updateDateTime(str(cliId.decode()), lseen)
