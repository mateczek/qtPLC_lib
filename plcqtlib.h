/*Ten plik jest częścią biblioteki plcQtLib.
    plcQtLib jest wolnym oprogramowaniem; możesz go rozprowadzać dalej
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
#ifndef PLCQTLIB_H
#define PLCQTLIB_H
#include<QtCore>
#include<functional>
#include<cstdint>

#if defined(__MINGW32__)||defined(__MINGW64__)
    typedef uint8_t PlcBYTE; // 1byte
    typedef uint16_t PlcWORD; // 2bytes
    typedef uint32_t PlcDWORD; //4bytes
#else
    typedef u_int8_t PlcBYTE; // 1byte
    typedef u_int16_t PlcWORD; // 2bytes
    typedef u_int32_t PlcDWORD; //4bytes
#endif

int PlcBYTEtoInt(PlcBYTE data);
int PlcWORDtoInt(PlcWORD data);
int PlcDWORDtoInt(PlcDWORD data);
struct _daveConnection;

class PlcQtLib:public QThread{
    Q_OBJECT

    QString ip; int dbNr; int dbSize;
    int Rack,Slot,sleepTime;
    unsigned char* dbBlock;

    _daveConnection *dc;
    QMutex mutex;
    void run();
    QQueue<std::function<bool()>> kolejkaZadan;
    bool _stopThread=false;
    bool plcConnect=false;
public:
    //s7-300    _Rack=0 _Slot =2
    //s7-1500   _Rack=0 _Slot =1
    PlcQtLib( QString _ip,int _dbNr, int _dbSize,unsigned char*_buf=nullptr, int _Slot=2,int _Rak=0, int _sleepTime=500);

    ~PlcQtLib();
    bool plcStatus(){
        return plcConnect;
    }
    void registerDb(unsigned char* buff){
        dbBlock=buff;
    }

    //interface function/slot
public slots:
    void writeByte(int dbb,int data);
    void writeWord(int dbw,int data);
    void writeDword(int dbd,int data);
    void setBit(int dbx,int bitNr);
    void clearBit(int dbx,int bitNr);
    void stopThread();

    //thread function
private:
    bool _writeByte(int dbb,int data);
    bool _writeWord(int dbw,int data);
    bool _writeDword(int dbd,int data);
    bool _setBit(int dbx,int bitNr);
    bool _clearBit(int dbx,int bitNr);
signals:
    void connected(bool stat);
    void dataReady();

};

#endif // PLCQTLIB_H
