
# Definition of Message class to easy work with the protocol by adding removing getting messages
class Message:

    ID:int = -1
    TOCLIENT:str = None
    FROMCLIENT:str = None
    TYPE:str = ''
    CONTENT = None

    def __init__(self,toc:str,frc:str,typ:str,content):

        self.TOCLIENT = toc
        self.FROMCLIENT = frc
        self.TYPE = typ
        self.CONTENT = content
