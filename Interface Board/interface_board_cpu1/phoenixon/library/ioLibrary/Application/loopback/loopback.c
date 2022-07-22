#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"

#include "F28x_Project.h"

#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

#pragma CODE_SECTION(loopback_tcps, ".TI.ramfunc");
#pragma CODE_SECTION(loopback_tcpc, ".TI.ramfunc");
#pragma CODE_SECTION(loopback_udps, ".TI.ramfunc");

int32_t tcp_ret;
uint16_t tcp_size = 0;
uint8_t tcp_destip[4];
uint16_t tcp_destport;

uint16_t gdb_test_size;
uint16_t gdb_test_flag;

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   //uint16_t size = 0, sentsize=0;

#ifdef _LOOPBACK_DEBUG_
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _LOOPBACK_DEBUG_
			getSn_DIPR(sn, destip);
			getSn_DIPR(sn, tcp_destip);
			tcp_destport = destport = getSn_DPORT(sn);

			printf("%d:Connected - %d.%d.%d.%d : %u\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
#if 1
         if(gdb_test_flag) {
             gdb_test_flag = 0;
             ret = send(sn, buf, gdb_test_size);
             printf("%d:send size - %u, sent size - %u\r\n",sn, gdb_test_size, ret);
             if(ret < 0)
             {
                 printf("%d:close 1\r\n",sn);
                 close(sn);
                 return ret;
             }
         }
#else
		 if((tcp_size = size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
		    printf("%d:recv size - %u\r\n",sn, tcp_size);
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			tcp_ret = ret = recv(sn, buf, size);
			printf("%d:read size - %lu\r\n",sn, tcp_ret);
			if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
			size = (uint16_t) ret;
			sentsize = 0;

			printf("%d:f write - %u\r\n",sn, hal_f_write(buf, size));

			while(size != sentsize)
			{
				ret = send(sn, buf+sentsize, size-sentsize);
				printf("%d:send size - %u, sent size - %u\r\n",sn, size, sentsize);
				if(ret < 0)
				{
				    printf("%d:close 1\r\n",sn);
					close(sn);
					return ret;
				}
				sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
			}
         }
#endif
         break;
      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%u]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}


int32_t loopback_tcpc(uint8_t sn, uint8_t* buf, uint8_t* destip, uint16_t destport)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0, sentsize=0;
   uint8_t host_ip[4];
   uint16_t host_port[4];

   // Destination (TCP Server) IP info (will be connected)
   // >> loopback_tcpc() function parameter
   // >> Ex)
   //	uint8_t destip[4] = 	{192, 168, 0, 214};
   //	uint16_t destport = 	5000;

   // Port number for TCP client (will be increased)
   static uint16_t any_port = 	50000;

   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
#ifdef _LOOPBACK_DEBUG_
			printf("%d:Connected to - %d.%d.%d.%d : %lu\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
         }

         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
		 if((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
			ret = recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

			if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
			size = (uint16_t) ret;
			sentsize = 0;

			// Data sentsize control
			while(size != sentsize)
			{
				ret = send(sn, buf+sentsize, size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
				if(ret < 0) // Send Error occurred (sent data length < 0)
				{
					close(sn); // socket close
					return ret;
				}
				sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
			}
         }
		 //////////////////////////////////////////////////////////////////////////////////////////////
         break;

      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;

      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Try to connect to the %d.%d.%d.%d : %lu\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
    	 if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
         break;

      case SOCK_CLOSED:
    	  close(sn);
    	  if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){
         if(any_port == 0xffff) any_port = 50000;
         return ret; // TCP socket open with 'any_port' port number
        } 
#ifdef _LOOPBACK_DEBUG_
    	 //printf("%d:TCP client loopback start\r\n",sn);
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}

int32_t udp_ret;
uint16_t udp_size = 0;

int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((udp_size = size = getSn_RX_RSR(sn)) > 0)
         {
             printf("%d:recv size - %u\r\n",sn, size);
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            udp_ret = ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            printf("%d:recv from - %d.%d.%d.%d : %u\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
            printf("%d:read size - %lu\r\n",sn, ret);
            if(ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
               printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
               if(ret < 0)
               {
#ifdef _LOOPBACK_DEBUG_
                  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Opened, UDP loopback, port [%u]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}

#endif
