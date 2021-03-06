



class Message:

    ID:int = -1
    TOCLIENT:str = None
    FROMCLIENT:str = None
    TYPE:str = ''
    CONTENT = None

    def __init__(self,toC:str,frC:str,type:str,content):

        self.TOCLIENT = toC
        self.FROMCLIENT = frC
        self.TYPE = type
        self.CONTENT = content