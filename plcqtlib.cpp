/*Ten plik jest częścią biblioteki plcQtLib.

    plcQtLib jest wolnym oprogramowaniem; możesz ją rozprowadzać dalej
    i/lub modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
    wydanej przez Fundację Wolnego Oprogramowania - według wersji 2 tej
    Licencji lub którejś z późniejszych wersji.

    Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
    użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
    gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
    ZASTOSOWAŃ. W celu uzyskania bliższych informacji sięgnij do
    Powszechnej Licencji Publicznej GNU.

    Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
    Powszechnej Licencji Publicznej GNU (GNU General Public License);
    jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
    Place, Fifth Floor, Boston, MA  02110-1301  USA
*/
//(C) mateczek marcof@interia.pl 2016
/*This file is part of plcQtLib.

    plcQtLib is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    plcQtLib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "plcqtlib.h"
#include<QTcpSocket>
#include<nodave.h>

PlcQtLib::PlcQtLib(QString _ip, int _dbNr, int _dbSize, unsigned char *_buf, int _Slot, int _Rack, int _sleepTime){
   ip=_ip;
   dbNr=_dbNr;
   dbSize=_dbSize;
   dbBlock=_buf;
   sleepTime=_sleepTime;
   Slot=_Slot;
   Rack=_Rack;
   sleepTime=_sleepTime;
   plcConnect=false;
}

PlcQtLib::~PlcQtLib()
{
    _stopThread=true;
}

//funkcje interfejsu rejestrujące wątki
void PlcQtLib::writeByte(int dbb, int data)
{
    QMutexLocker m(&mutex);
    kolejkaZadan.enqueue(std::bind(&PlcQtLib::_writeByte,this,dbb,data));
}

void PlcQtLib::writeWord(int dbw, int data)
{
    QMutexLocker m(&mutex);
    kolejkaZadan.enqueue(std::bind(&PlcQtLib::_writeWord,this,dbw,data));
}

void PlcQtLib::writeDword(int dbd, int data)
{
    QMutexLocker m(&mutex);
    kolejkaZadan.enqueue(std::bind(&PlcQtLib::_writeDword,this,dbd,data));
}

void PlcQtLib::setBit(int dbx, int bitNr)
{
    QMutexLocker m(&mutex);
    kolejkaZadan.enqueue(std::bind(&PlcQtLib::_setBit,this,dbx,bitNr));
}

void PlcQtLib::clearBit(int dbx, int bitNr)
{
    QMutexLocker m(&mutex);
    kolejkaZadan.enqueue(std::bind(&PlcQtLib::_clearBit,this,dbx,bitNr));
}

void PlcQtLib::stopThread()
{
    _stopThread=true;
}

//funkcje wątku
bool PlcQtLib::_writeByte(int dbb, int data){
   PlcBYTE _data=data;
   int res=daveWriteBytes(dc,daveDB,dbNr,dbb,1,&_data);
   return !res;

}

bool PlcQtLib::_writeWord(int dbw, int data)
{
    PlcWORD _data=daveSwapIed_16(data);
    int res= daveWriteBytes(dc,daveDB,dbNr,dbw,2,&_data);
    return !res;

}

bool PlcQtLib::_writeDword(int dbd, int data)
{
    PlcDWORD _data=daveSwapIed_32(data);
    int res=daveWriteBytes(dc,daveDB,dbNr,dbd,4,&_data);
    return !res;

}

bool PlcQtLib::_setBit(int dbx, int bitNr)
{
    return !daveSetBit(dc,daveDB,dbNr,dbx,bitNr);
}

bool PlcQtLib::_clearBit(int dbx, int bitNr)
{
    return !daveClrBit(dc,daveDB,dbNr,dbx,bitNr);
}
//funkcja run
void PlcQtLib::run()
{
    QTcpSocket socket;
    socket.connectToHost(ip,102);
    if(!socket.waitForConnected(500)){
        socket.disconnect();
        emit connected(false);
        return;
    }
    _daveOSserialType fds;
    daveInterface *di;
    fds.wfd =(HANDLE)socket.socketDescriptor();
    fds.rfd=fds.wfd;
    plcConnect=false;
    if (fds.rfd>0){
        di = daveNewInterface(fds, "IF1",0, 122, daveSpeed187k);
        daveSetTimeout(di,5000000);
        dc=daveNewConnection(di,2,Rack,Slot);
        if(daveConnectPLC(dc)==0)
            plcConnect=true;
        else
            plcConnect=false;
    }
    if(!plcConnect){
        socket.disconnectFromHost();
        daveDisconnectPLC(dc);
        daveFree(dc);
        emit connected(false);
        return;
    }
    emit connected(true);
    while(!_stopThread){
        while (!kolejkaZadan.empty()) {
            QMutexLocker m(&mutex);
            plcConnect=kolejkaZadan.dequeue()();
            if(!plcConnect) break;
        }
        if(!plcConnect) break;
        plcConnect=!daveReadBytes(dc,daveDB,dbNr,0,dbSize,dbBlock);
        if(!plcConnect) break;
        emit dataReady();
        if(sleepTime) QThread::msleep(sleepTime);
    }
    socket.disconnectFromHost();
    daveDisconnectPLC(dc);
    daveFree(dc);
    emit connected(false);
}


int PlcBYTEtoInt(PlcBYTE data)
{
   return data;
}

int PlcWORDtoInt(PlcWORD data)
{
   return daveSwapIed_16(data);
}

int PlcDWORDtoInt(PlcDWORD data)
{
   return daveSwapIed_32(data);
}

