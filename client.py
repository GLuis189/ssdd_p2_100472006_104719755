from enum import Enum
import argparse
import socket
import threading
import os
import sys

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

        # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _socket = None
    _listen_socket = None
    _listen_thread = None

    # ******************** METHODS *******************
    @staticmethod
    def handle_requests():
        while True:
            conn, addr = client._listen_socket.accept()
            # handle the request here
            conn.close()

    @staticmethod
    def register(user):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            message = f"REGISTER {user}\0"
            client._socket.sendall(message.encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > REGISTER OK")
                return client.RC.OK
            elif response == '1':
                print("c > USERNAME IN USE")
                return client.RC.USER_ERROR
            else:
                print("c > REGISTER FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()

    @staticmethod
    def unregister(user):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall(f"UNREGISTER {user}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > UNREGISTER OK")
                return client.RC.OK
            elif response == '1':
                print("c > USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            else:
                print("c > UNREGISTER FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    @staticmethod
    def connect(user):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))

            client._listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._listen_socket.bind(('localhost', 0))  # bind to a free port
            listen_port = client._listen_socket.getsockname()[1]
            client._listen_socket.listen(1)

            client._listen_thread = threading.Thread(target=client.handle_requests)
            client._listen_thread.start()

            client._socket.sendall(f"CONNECT {user} {listen_port}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > CONNECT OK")
                return client.RC.OK
            elif response == '1':
                print("c > CONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > USER ALREADY CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("c > CONNECT FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()

    @staticmethod
    def disconnect(user):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall(f"DISCONNECT {user}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > DISCONNECT OK")
                client._listen_thread.join()  # stop the listening thread
                client._listen_socket.close()  # close the listening socket
                return client.RC.OK
            elif response == '1':
                print("c > DISCONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > DISCONNECT FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("c > DISCONNECT FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    @staticmethod
    def publish(fileName, description):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall(f"PUBLISH {fileName} {description}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > PUBLISH OK")
                return client.RC.OK
            elif response == '1':
                print("c > PUBLISH FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > PUBLISH FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("c > PUBLISH FAIL, CONTENT ALREADY PUBLISHED")
                return client.RC.USER_ERROR
            else:
                print("c > PUBLISH FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    @staticmethod
    def delete(fileName):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall(f"DELETE {fileName}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                print("c > DELETE OK")
                return client.RC.OK
            elif response == '1':
                print("c > DELETE FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > DELETE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("c > DELETE FAIL, CONTENT NOT PUBLISHED")
                return client.RC.USER_ERROR
            else:
                print("c > DELETE FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    @staticmethod
    def listusers():
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall("LIST_USERS\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                num_users = int(client._socket.recv(1024).decode())
                print("c > LIST_USERS OK")
                for _ in range(num_users):
                    user_info = client._socket.recv(1024).decode().split()
                    print(f"{user_info[0]} {user_info[1]} {user_info[2]}")
                return client.RC.OK
            elif response == '1':
                print("c > LIST_USERS FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > LIST_USERS FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            else:
                print("c > LIST_USERS FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    @staticmethod
    def listcontent(user):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client._socket.connect((client._server, client._port))
            client._socket.sendall(f"LIST_CONTENT {user}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                num_files = int(client._socket.recv(1024).decode())
                print("c > LIST_CONTENT OK")
                for _ in range(num_files):
                    file_info = client._socket.recv(1024).decode().split('\0', 1)
                    print(f"{file_info[0]} \"{file_info[1]}\"")
                return client.RC.OK
            elif response == '1':
                print("c > LIST_CONTENT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif response == '2':
                print("c > LIST_CONTENT FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            elif response == '3':
                print("c > LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            else:
                print("c > LIST_CONTENT FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()


    import os

    @staticmethod
    def getfile(user, remote_file_name, local_file_name):
        try:
            client._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # assuming _server and _port are the IP and port of the other client
            client._socket.connect((client._server, client._port))  
            client._socket.sendall(f"GET_FILE {remote_file_name}\0".encode())
            response = client._socket.recv(1024).decode()
            if response == '0':
                with open(local_file_name, 'wb') as f:
                    while True:
                        data = client._socket.recv(1024)
                        if not data:
                            break
                        f.write(data)
                print("c > GET_FILE OK")
                return client.RC.OK
            elif response == '1':
                print("c > GET_FILE FAIL, FILE NOT EXIST")
                return client.RC.USER_ERROR
            else:
                print("c > GET_FILE FAIL")
                return client.RC.ERROR
        except Exception as e:
            print(f"Error: {str(e)}")
            return client.RC.ERROR
        finally:
            client._socket.close()
            if response != '0' and os.path.exists(local_file_name):
                os.remove(local_file_name)  # delete the local file if the transfer was not successful


    @staticmethod
    def parseArguments(argv):
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()
        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False
        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False
        client._server = args.s
        client._port = args.p
        return True

    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c > ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;
        
        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return
        #  Write code here
        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])