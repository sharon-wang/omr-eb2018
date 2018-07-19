/*******************************************************************************
 * Copyright (c) 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

 #include <cstring>
 #include <queue>
 #include <iostream>
 #include <stdlib.h>
 #include <fstream>
 #include <string>
 #include <vector>

 #include "omrthread.h"

 // gRPC includes
 #include "infra/Assert.hpp"
 #include <grpc/grpc.h>
 #include <grpcpp/server.h>
 #include <grpcpp/server_builder.h>
 #include <grpcpp/server_context.h>
 #include <grpcpp/security/server_credentials.h>
 #include <grpc/support/log.h>
 #include "imperium.grpc.pb.h"

 #include <grpcpp/grpcpp.h>

 // Server
 using grpc::Server;
 using grpc::ServerBuilder;
 using grpc::ServerContext;
 using grpc::ServerReaderWriter;
 using grpc::Status;
 using grpc::StatusCode;

 // Client
 using grpc::Channel;
 using grpc::ClientContext;
 using grpc::ClientReader;
 using grpc::ClientReaderWriter;
 using grpc::ClientWriter;

 using imperium::ClientMessage;
 using imperium::ServerResponse;
 using imperium::ImperiumRPC;

// include header files for proto stuff HERE

namespace OMR
{
   namespace Imperium
   {
      class ServerChannel final : public ImperiumRPC::Service
      {
         public:
         ServerChannel();
         ~ServerChannel();

         Status SendMessage(ServerContext* context,
                           ServerReaderWriter<ServerResponse, ClientMessage>* stream) override;

         // Server-facing
         bool RunServer(const char * port);
      };

      class ClientChannel
      {
         public:
         enum MonitorStatus {
         SHUTTING_DOWN,
         SHUTDOWN_COMPLETE,
         INITIALIZING,
         INITIALIZING_COMPLETE, // INITIALIZATION ******
         NO_JOBS_LEFT
         };

         ClientChannel();
         ~ClientChannel();

         typedef std::shared_ptr<ClientReaderWriter<ClientMessage, ServerResponse>> sharedPtr;

         bool initClient(const char * port);
         void generateIL(const char * fileName);
         void createWriterThread();
         bool addMessageToTheQueue(ClientMessage message);
         static std::vector<std::string> readFilesAsString(char * fileNames [], int numOfFiles);

         bool isQueueEmpty();
         bool isShutdownComplete();
         void signalNoJobsLeft();
         void waitForThreadCompletion();
         ClientMessage constructMessage(std::string file, uint64_t address);

         // TEST
         void createReaderThread();

         void waitForMonitor();
         void SendMessage();

         private:
         omrthread_monitor_t _monitor;
         MonitorStatus _monitorStatus;
         std::queue<ClientMessage> _queueJobs;
         std::unique_ptr<ImperiumRPC::Stub> stub_;
         sharedPtr _stream;

         void shutdown();
         omrthread_t attachSelf();
         static int writerThread(void * data);
         static int readerThread(void *data);
         ClientMessage getNextMessage();
         void handleWrite();
         void handleRead();
      };
   }
} // namespace OMR

 // namespace TR
 // {
 //    class ServerChannel : public OMR::ServerChannel
 //       {
 //       public:
 //          ServerChannel()
 //            : OMR::ServerChannel()
 //            { }
 //          virtual ~ServerChannel()
 //            { }
 //       };
 // } // namespace TR
