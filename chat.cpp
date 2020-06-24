#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <cstdlib>

using namespace std;
const int sizeBuffers = 4096;

class SerialPort
{
private:
    // Declaração da porta serial.
    HANDLE hComm;

    char Buffer[4096];
    char Byte;

    // Variáveis de Controle da Porta Serial.
    DWORD dwToRead;
    DWORD dwRead;
    DWORD dwWritten;
    DWORD baudRate;
    BYTE parity;
    BYTE stopBits;
    BYTE byteSize;

public:
    // Construtor da classe SerialPort.
    SerialPort();

    // Abertura e fechamento da porta serial.
    BOOL openSerialPort(string, string);
    void closeSerialPort();

    // Escrita e leitura do buffer de dados.
    BOOL writeABuffer(char *, DWORD);
    char *readABuffer();

    // Demais parâmetros de controle da porta serial.
    vector<string> checkSerialPort();
    unsigned int getBufferSize();
};

SerialPort::SerialPort()
{
    // Variável que que receberá a porta serial aberta.
    hComm = NULL;

    // Valores padrão.
    baudRate = CBR_9600;   // CBR_38400, CBR_9600
    parity = NOPARITY;     // 0 - 4 = no, odd, even, mark, space
    stopBits = ONESTOPBIT; // 0 - 2 = 1, 1.5, 2
    byteSize = 8;

    // Configuração dos tamanhos dos buffers de leitura e escrita.
    dwToRead = 4096;
}

BOOL SerialPort::openSerialPort(string asPort, string asBaudRate)
{
    string asCommPort;

    // Verifica se há uma porta serial ainda aberta caso exista ela é fechada.
    if (hComm != NULL)
    {
        // Fecha a porta serial.
        CloseHandle(hComm);
        hComm = NULL;
    }

    // Configuração do nome da porta serial selecionada.
    asCommPort = "\\\\.\\" + asPort;

    // Configurações do Baud Rate da porta serial selecionada.
    if (asBaudRate == "256000")
        baudRate = CBR_256000;
    if (asBaudRate == "128000")
        baudRate = CBR_128000;
    if (asBaudRate == "115200")
        baudRate = CBR_115200;
    if (asBaudRate == "57600")
        baudRate = CBR_57600;
    if (asBaudRate == "56000")
        baudRate = CBR_56000;
    if (asBaudRate == "38400")
        baudRate = CBR_38400;
    if (asBaudRate == "19200")
        baudRate = CBR_19200;
    if (asBaudRate == "14400")
        baudRate = CBR_14400;
    if (asBaudRate == "9600")
        baudRate = CBR_9600;
    if (asBaudRate == "4800")
        baudRate = CBR_4800;
    if (asBaudRate == "2400")
        baudRate = CBR_2400;
    if (asBaudRate == "1200")
        baudRate = CBR_1200;

    // Abertura da porta serial selecionada.
    hComm = CreateFile(asCommPort.c_str(),           // Ponteiro para a porta selecionada.
                       GENERIC_READ | GENERIC_WRITE, // Modo de acesso (escrita ou leitura).
                       0,                            // Não utilizado.
                       0,                            // Não utilizado.
                       CREATE_ALWAYS,                // Como a porta será criada.
                       NULL,                         // Não utilizado.
                       0);                           // Não utilizado.

    // Faz a verificação se a porta foi aberta corretamente.
    if (hComm != INVALID_HANDLE_VALUE)
    {
        // Configuração dos Timeouts da Porta Serial.
        COMMTIMEOUTS CommTimeouts;

        // Atribuição dos valores definidos para os Timeouts.
        CommTimeouts.ReadIntervalTimeout = MAXDWORD;
        CommTimeouts.ReadTotalTimeoutMultiplier = 0;
        CommTimeouts.ReadTotalTimeoutConstant = 0;
        CommTimeouts.WriteTotalTimeoutMultiplier = 0;
        CommTimeouts.WriteTotalTimeoutConstant = 0;

        // Verificação se os parâmetros de configuração foram aceitos.
        if (SetCommTimeouts(hComm, &CommTimeouts) == 0)
        {
            printf("\nERRO AO ALTERAR DADOS DE CONFIGURAÇÃO DA PORTA SERIAL");
            CloseHandle(hComm);
            hComm = NULL;

            return (false);
        }

        // Configurações dos parâmetros de funcionamento da Porta Serial.
        DCB dcb = {0};

        // Verificação se foi possível obter os parâmetros de configuração originais da Porta Serial.
        if (GetCommState(hComm, &dcb) == 0)
        {
            printf("\nERRO AO OBTER DADOS DE CONFIGURAÇÃO DA PORTA SERIAL");
            CloseHandle(hComm);
            hComm = NULL;

            // Sinaliza erro na abertura da porta serial.
            return (false);
        }

        // Estrutura de configuração da porta serial.
        dcb.DCBlength = sizeof(dcb); // sizeof(DCB)

        // Atribuição dos parâmetros (DEFAULT) de configuração da Porta Serial.
        dcb.BaudRate = baudRate;   // current baud rate
        dcb.fBinary = true;        // binary mode, no EOF check
        dcb.fParity = true;        // enable parity checking
        dcb.ByteSize = 8;          // number of bits/byte, 4-8
        dcb.Parity = NOPARITY;     // 0 - 4 = no, odd, even, mark, space
        dcb.StopBits = ONESTOPBIT; // 0, 1, 2 = 1, 1.5, 2

        // Demais parâmetros opcional.
        dcb.fOutxCtsFlow = false;              // CTS output flow control
        dcb.fOutxDsrFlow = false;              // DSR output flow control
        dcb.fDtrControl = DTR_CONTROL_DISABLE; // DTR flow control type
        dcb.fDsrSensitivity = false;           // DSR sensitivity
        dcb.fTXContinueOnXoff = false;         // XOFF continues Tx            2
        dcb.fOutX = false;                     // XON/XOFF out flow control
        dcb.fInX = false;                      // XON/XOFF in flow control
        dcb.fErrorChar = false;                // enable error replacement     1
        dcb.fNull = false;                     // enable null stripping
        dcb.fRtsControl = RTS_CONTROL_DISABLE; // RTS flow control
        dcb.fAbortOnError = false;             // abort reads/writes on error
        dcb.fDummy2 = false;                   // reserved                     1
        dcb.XonLim = 0;                        // transmit XON threshold       1
        dcb.XoffLim = 0;                       // transmit XOFF threshold      1
        dcb.XonChar;                           // Tx and Rx XON character
        dcb.XoffChar;                          // Tx and Rx XOFF character
        dcb.ErrorChar;                         // error replacement character
        dcb.EofChar;                           // end of input character
        dcb.EvtChar;                           // received event character
        // dcb.wReserved;                          // not currently used           1
        dcb.wReserved1; // reserved; do not use

        // Verificação se os parâmetros de configuração foram aceitos.
        if (SetCommState(hComm, &dcb) == 0)
        {
            printf("\nERRO AO ALTERAR DADOS DE CONFIGURAÇÃO DA PORTA SERIAL");
            CloseHandle(hComm);
            hComm = NULL;

            return (false);
        }
    }
    else
    {
        printf("\nERRO AO ABRIR A PORTA SERIAL");
        CloseHandle(hComm);
        hComm = NULL;

        return (false);
    }

    // Descarta caracteres presentes na porta e termina processos pendentes de leitura e transmissão.
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    return (true);
}

// ---------------------------------------------------------------------------

BOOL SerialPort::writeABuffer(char *Buffer, DWORD dwToWrite)
{
    OVERLAPPED osWrite = {0};

    if (hComm != NULL)
    {
        // Create this writes OVERLAPPED structure hEvent.
        osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (osWrite.hEvent == NULL)
        {
            return (false); // Error creating overlapped event handle.
        }

        if (!WriteFile(hComm, Buffer, dwToWrite, &dwWritten, &osWrite))
        {
            printf("\nERRO AO ENVIAR DADOS PARA A PORTA SERIAL");
        }

        CloseHandle(osWrite.hEvent);
    }
    else
    {
        printf("\nERRO AO ABRIR A PORTA SERIAL");
        CloseHandle(hComm);
        return (FALSE);
    }

    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    return (true);
}

char *SerialPort::readABuffer()
{
    OVERLAPPED osRead = {0};
    if (hComm != NULL)
    {
        // Create this writes OVERLAPPED structure hEvent.
        osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (osRead.hEvent != NULL)
        {
            strcpy(Buffer, "\x0"); // Limpa o que tinha no buffer
            if (ReadFile(hComm, &Buffer, dwToRead, &dwRead, &osRead))
            {
                // Encerra corretamente a string para não retornar lixo.
                Buffer[dwRead] = '\0';
            }
            else
            {
                printf("\nERRO AO LER DADOS DA PORTA SERIAL");
            }
        }
        else
        {
            printf("\nERRO AO ABRIR A PORTA SERIAL");
            CloseHandle(hComm);
            return (FALSE);
        }

        CloseHandle(osRead.hEvent);
    }
    else
    {
        printf("\nERRO AO ABRIR A PORTA SERIAL");
        CloseHandle(hComm);
        return (FALSE);
    }

    // Finaliza todas as pendências de escrita e leitura da porta serial selecionada.
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    // Retorna o buffer recebido.
    return (Buffer);
}

// Retorna o tamanho do buffer ----------------------------------------------
unsigned int SerialPort::getBufferSize()
{
    // Obtém o número de bytes no buffer serial a serem lidos.
    return (dwRead);
}

// Fecha porta Serial ----------------------------------------------------------
void SerialPort::closeSerialPort()
{
    // Fecha a porta serial aberta.
    if (hComm != NULL)
    {
        SetCommMask(hComm, 0L);

        CloseHandle(hComm);
        hComm = NULL;
    }
}

// Tela de informacoes -----------------------------------------------------------
void header(char *msg, char *portas, char *baudrate)
{
    system("cls");
    printf("\n-----------------------------------------------------------------------------------------------");
    printf("\n %s                         ", msg);
    printf("\n-----------------------------------------------------------------------------------------------");
    printf("\n PORTA: %s                                                               BAUDRATE: %s          ", portas, baudrate);
    printf("\n-----------------------------------------------------------------------------------------------");
}

// Codigo principal  ------------------------------------------------------------
int main(void)
{
    char bufferTx[sizeBuffers];
    char bufferRx[sizeBuffers];
    char buffer[sizeBuffers];
    char portas[15];
    char baudrate[15];
    char ch;
    unsigned short ADCResult = 0;
    int i = 0;

    // Inicialização da porta serial.
    SerialPort *porta;
    porta = new SerialPort();

    header("PARAMETROS DE CONFIGURACAO DA PORTA SERIAL", "", "");

    // Obtém a porta serial a ser conectada.
    printf("\n>> INFORME UMA PORTA PARA CONECTAR (Ex: COM3 / ENTER - VALIDAR PORTA / ESC - CANCELAR)");
    printf("\n>> PORTA: ");

    i = 0;
    ch = '\0';
    while (ch != 13)
    {
        kbhit();
        ch = getch();
        printf("%c", ch);

        portas[i] = ch;
        i++;

        if (ch == 27)
        {
            printf("\n>> INFORME UMA NOVA PORTA!");
            printf("\n>> PORTA: ");

            i = 0;
            ch = '\0';
            strcpy(portas, " ");
        }
    }

    i--;
    portas[i] = '\0';
    printf("\nPORTA ESCOLHIDA: %s\n\n", portas);
    printf("\nINFORME A TAXA DE BAUDRATE (Ex: 9600 / ENTER - VALIDAR BAUDRATE / ESC - CANCELAR)");
    printf("\nBAUDRATE: ");

    i = 0;
    ch = '\0';
    while (ch != 13)
    {
        kbhit();
        ch = getch();
        printf("%c", ch);

        baudrate[i] = ch;
        i++;

        if (ch == 27)
        {
            printf("\nBAUDRATE: ");

            i = 0;
            ch = '\0';
            strcpy(baudrate, " ");
        }
    }

    i--;
    baudrate[i] = '\0';
    printf("\n>> BAUDRATE: %s\n", baudrate);
    header("SAIDA", portas, baudrate);
    porta->openSerialPort(portas, baudrate);
    printf("\n");
    i = 0;
    ch = '\0';
    strcpy(bufferRx, "\x0");
    strcpy(bufferTx, "\x0");
    while (ch != 27) // Esc
    {                // vai ficar lendo a entrada USB...
        strcpy(bufferRx, porta->readABuffer());

        while (porta->getBufferSize() != 0)
        {
            Sleep(200);
            strcat(bufferRx, porta->readABuffer());
            printf("buffer size>> %i\n", porta->getBufferSize());
            if (porta->getBufferSize() == 0)
            {
                // Mostra os dados recebidos.
                printf(" - %s\n", bufferRx);

                strcpy(bufferRx, "\x0"); // limpa o bufer
            }
        }
        if (kbhit()) // Se algo for pressionado grava no buffer TX
        {
            ch = getch();
            printf("%c", ch); // mostra pro usuario o que ele escreveu
            if (ch != 8)      // funcao backspace incrementada
            {
                if (ch != 13) // se for algo diferente de enter grava no buffer
                {
                    bufferTx[i] = ch;
                    i++;
                }
                if (ch == 13 && i > 0) // Enter
                {
                    // i--; //
                    bufferTx[i] = '\0';

                    porta->writeABuffer(bufferTx, i); // envia a informacao

                    // Limpa as variaveis em uso
                    i = 0;
                    ch = '\0';
                    strcpy(bufferTx, "\x0");
                    printf("\n");
                    Sleep(500);
                }
            }
            else
            {
                if (i > 0)
                    i--;
            }
        }
        strcpy(bufferRx, "\x0");
    }

    // ------------------------------------------------------

    printf("\n - Saindo...\n\n\n");

    // Fecha a porta serial.
    porta->closeSerialPort();
    porta = NULL;
    delete porta;

    return 0;
}
