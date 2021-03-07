import sqlite3
import Client
import Message


class SqlLite:

    CONN = None
    CUR = None

    def __init__(self,serverfilename:str):
        self.CONN = self.__createConnection(serverfilename)
        self.CUR = self.CONN.cursor()
        self.__createTables()

    def getMsg(self, id:int):
        sql = 'SELECT * FROM Messages WHERE ID = ? ;'
        res = self.CUR.execute(sql, (id,))
        return res.fetchall()

    def getCliByID(self, id:str):

        sql = 'SELECT * FROM Clients WHERE ID = ? ;'
        self.CUR.execute(sql, (id,))
        return self.CUR.fetchall()

    def getCliByName(self, name:str):
        sql = 'SELECT * FROM Clients WHERE Name = ? ;'
        self.CUR.execute(sql,(name,))
        return self.CUR.fetchall()

    def getPKeyCliByName(self, name:str):
        sql = 'SELECT ID,PublicKey FROM Clients WHERE Name = ? ;'
        self.CUR.execute(sql, (name,))
        return self.CUR.fetchall()

    def getAllCli(self):
        self.CUR.execute('''SELECT * FROM Clients ;''')
        return self.CUR.fetchall()

    def getMsgByToClId(self,id:str):

        self.CUR.execute('''SELECT FromClient,ID,Type,Content FROM Messages WHERE ToClient = ? ;''',(id,))
        return self.CUR.fetchall()

    def deleteMsgByToClId(self, toID:str):

        sql = 'DELETE FROM Messages WHERE ToClient = ? ;'
        self.CUR.execute(sql, (toID,))
        self.CONN.commit()

    def insertMsg(self, msg:Message.Message):

        try:
            sql = ''' INSERT OR IGNORE INTO Messages(ToClient,FromClient,Type,Content)
                    VALUES(?,?,?,?) ;'''
            self.CUR.execute(sql,(msg.TOCLIENT,msg.FROMCLIENT,msg.TYPE,msg.CONTENT,))
            self.CONN.commit()

            msg.ID = self.CUR.lastrowid
            return msg.ID
        except sqlite3.Error as e:
            print(e)
        return -1

    def insertCli(self, cli:Client.Client):

        anotherOne = str(self.getCliByName(cli.NAME))
        if len(anotherOne) < 3:
            try:

                sql = ''' INSERT INTO Clients(ID,Name,PublicKey,LastSeen)
                        VALUES(?,?,?,?) ;'''
                self.CUR.execute(sql,(cli.ID,cli.NAME,cli.PUBLICKEY,cli.LASTSEEN,))
                self.CONN.commit()
                return cli.ID
            except sqlite3.Error as e:
                print(e)
        return -1

    def updateDateTime(self,id:str,datetime):
        try:
            sql = ''' UPDATE Clients
                    SET LastSeen = ?
                    WHERE ID = ?'''
            self.CUR.execute(sql,(datetime,id,))
            self.CONN.commit()
        except sqlite3.Error as e:
            print(e)

    def __createConnection(self,db_file:str):
            """ create a database connection to the SQLite database
                specified by db_file
            :param db_file: database file
            :return: Connection object or None
            """
            conn = None
            try:
                conn = sqlite3.connect(db_file,check_same_thread=False)

            except sqlite3.Error as e:
                print(e)

            return conn

    def __createTables(self):

        with self.CONN:
            try:

                sql = '''CREATE TABLE Clients(ID VARCHAR(16) PRIMARY KEY,Name VARCHAR(255),PublicKey VARCHAR(128),LastSeen DATETIME ) ;'''
                self.CUR.execute(sql)
                self.CONN.commit()

            except sqlite3.Error as e:
                print(e)


            try:

                sql = '''CREATE TABLE Messages(ID INTEGER PRIMARY KEY AUTOINCREMENT,ToClient VARCHAR(16) NOT NULL, FromClient VARCHAR(16) NOT NULL,Type VARCHAR(1),Content BLOB, FOREIGN KEY(FromClient) REFERENCES Clients(ID),FOREIGN KEY(ToClient) REFERENCES Clients(ID)) ;'''
                self.CUR.execute(sql)
                self.CONN.commit()

            except sqlite3.Error as e:
                print(e)



    def closeConn(self):
        try:
            self.CUR.close()
            self.CONN.close()
        except sqlite3.Error as e:
            print(str(e))
