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
 * $Id: ClientDemo.cpp,v 1.3 2004/05/04 11:20:31 rtlammer Exp $
 *
 */

#include "SocketStreams.hpp"

int main (int argc, const char* argv[])
{
  //    assert (argc > 1);
  //    InetSockAddr<> sockAddr (argv[1], 45);
  //    std::cout << "InetAddr: " << AF<>::toString (sockAddr.host.address)
  //  	    << std::endl;
  //    std::cout << "Port: " << sockAddr.port.portNumber
  //  	    << std::endl;

  if ((argc < 2) || (argc > 3)) {
    std::cout << "usage: " << argv[0] << " <PortNr> [<HostNameOrInetAddr>]" << std::endl;
    return EXIT_FAILURE;
  }

  InetHost<> clientHost;
  if (argc == 3) {
    AF<>::Address inetAddr;
    if (AF<>::getAddress (argv[2], inetAddr)) {
      clientHost = InetHost<> (inetAddr);
    } else {
      clientHost = InetHost<> (argv[2]); /*: should be a valid host name */
    }
  }
  ClientSocket<> clientSocket (clientHost /* host */, argv[1] /* port */);
  assert (clientSocket.open ());
  MainPtr<ReadWriteSocket>::SubPtr rwSocket
    = clientSocket.connect ();
  assert (rwSocket->isOpen ());
  std::cout << "Client is sending:" << std::endl;

  iosockstream* socketStream = new iosockstream (rwSocket);

  const char* msg = "If it does...";
  *socketStream << msg << std::endl;
  std::cout << msg << std::endl;

  msg = "...it's really ok!";
  *socketStream << msg << std::endl;
  std::cout << msg << std::endl;


  delete socketStream;
  assert (clientSocket.close ());

  ClientSocket<> clientSocket2 (clientSocket.serverSocketAddress);
  assert (clientSocket2.open ());
  MainPtr<ReadWriteSocket>::SubPtr rwSocket2
    = clientSocket2.connect ();
  assert (rwSocket2->isOpen ());
  socketStream = new iosockstream (rwSocket2);
  assert (*socketStream);


  std::cout << "Client is receiving:" << std::endl;

  // *socketStream << std::noskipws;
  socketStream->unsetf (std::ios::skipws); // unset flag
  do {
    char c;
    *socketStream >> c;
    if (! *socketStream) break;
    std::cout << c;
  } while (true);
  std::cout << std::endl;

  assert (clientSocket2.close ());
}
