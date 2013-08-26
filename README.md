FTP
===

FTP Server and Client

    An FTP server which listens on a specified port for client request.
    
    An FTP client which can get connected to the FTP server when specified the server and the port.

    The FTP server and client have the provide following functionalities:-
        1) get file     : get a file (file name specified) from server current directory 
                          to client current directory.
        2) put file     : put a file (file name specified) to server current directory
                         	from client current directory.
        3) mget file    : gets multiple files (file name specified) from server current 
                        	directory to client current directory.
        4) mput file    : puts multiple files (file name specified) to server current 
                        	directory from client current directory.
        5) cd dir       : change server current directory.
        6) lcd dir      : change client local current directory.
        7) mget *       : mget with wildcard support.
        8) mput *       : mput with wildcard support.
        9) dir          : Show the content of servers current directory.
        10) ldir        : Show the content of clients current directory.
        11) rget dir    : Recursively get a directory (directory name specified) from server
                          current directory to client current directory.
        12) rput dir    : Recursively put a directory (directory name specified) to server
                          current directory from client current directory.
