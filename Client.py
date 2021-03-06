


class Client:

    ID:str = None
    NAME:str = ''
    PUBLICKEY:str = ''
    LASTSEEN:str = None

    def __init__(self,id:str,name:str,pKey:str,lSeen:str):

        self.ID = id
        self.NAME = name
        self.PUBLICKEY = pKey
        self.LASTSEEN = lSeen








