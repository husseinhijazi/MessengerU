
# Definition of Client class to easy work with the protocol by adding removing getting clients
class Client:

    ID:str = None
    NAME:str = ''
    PUBLICKEY:str = ''
    LASTSEEN:str = None

    def __init__(self,id:str,name:str,pkey:str,lseen:str):

        self.ID = id
        self.NAME = name
        self.PUBLICKEY = pkey
        self.LASTSEEN = lseen








