/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AnT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: ServerDemo.cpp,v 1.3 2004/05/04 14:25:28 rtlammer Exp $
 *
 */

#include "SocketStreams.hpp"
#include <string>

int main (int argc, const char* argv[])
{
  //    assert (argc > 1);
  //    InetSockAddr<> sockAddr (argv[1], 45);
  //    std::cout << "InetAddr: " << AF<>::toString (sockAddr.host.address)
  //  	    << std::endl;
  //    std::cout << "Port: " << sockAddr.port.portNumber
  //  	    << std::endl;

  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " <PortNr>" << std::endl;
    return EXIT_FAILURE;
  }

  // start a server
  ServerSocket<> serverSocket (argv[1]);
  std::cout << "server is running on host: "
            << serverSocket.socketAddress.host.hostName
            << std::endl;
  std::cout << "server IP address: "
            << AF<>::toString (serverSocket.socketAddress.host.address)
            << std::endl;

  assert (serverSocket.open ());
  assert (serverSocket.bind ());
  assert (serverSocket.listen ());
  AF<>::Socket clientSocket;
  std::cout << "Server: waiting for the client to connect..."
	    << std::endl;
  MainPtr<ReadWriteSocket>::SubPtr rwSocket
    = serverSocket.accept (&clientSocket);
  assert (rwSocket->isOpen ());

  iosockstream* socketStream = new iosockstream (rwSocket);

//    char buf[256];
//    int n = rwSocket->read (buf, 255);
//    buf[n] = '\0';

  std::cout << "Server is receiving:" << std::endl;

  // *socketStream << std::noskipws;
  socketStream->unsetf (std::ios::skipws); // unset flag
  do {
    char c;
    *socketStream >> c;
    if (! *socketStream) break;
    std::cout << c;
  } while (true);
  std::cout << std::endl;


  delete socketStream;

  MainPtr<ReadWriteSocket>::SubPtr rwSocket2
    = serverSocket.accept (&clientSocket);
  assert (rwSocket2->isOpen ());
  socketStream = new iosockstream (rwSocket2);
  assert (*socketStream);


  std::cout << "Server is sending:" << std::endl;

  const char* msg = "Ack...";
  *socketStream << msg << std::endl;
  std::cout << msg << std::endl;

  msg = "...from server.";
  *socketStream << msg << std::endl;
  std::cout << msg << std::endl;

  assert (serverSocket.close ());
}
