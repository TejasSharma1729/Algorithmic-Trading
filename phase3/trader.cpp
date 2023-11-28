// Listening to a given port no 8888 and printing the incoming messages
#include <iostream>
#include<fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include<mutex>
#include<thread>
#include<chrono>
#include"customdict.h"
#include"customheap.h"
#include"custom_queue.h"
using namespace std;

const int BUFFER_SIZE = 1024;
int NUM_THREADS = 2;
std::mutex printMutex;
// Structure to store client socket and its details

struct order{
    public:
    int thread;
    static int idc;
    int id;         //negative id means invalid
    int timeIN;
    int timeEXP;
    int price;
    int num;
    bool bs;        //1 if sell and 0 if buy
    string stock;
    string broker;
    order(){
        id=++idc;
    };
    order(char* ordre,int& end, int thr):thread(thr){
        id=++idc;
        broker="";
        string x="";
        int i=0;
        while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
            x.push_back(ordre[i]);            
            i++;
        }
        if(ordre[i]==0){
            end+=i;
            return;
        }
        i++;
        if(ordre[i]==13&&ordre[i]!=10)i++;
        
        timeIN=stoi(x);

        while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
            broker.push_back(ordre[i]);            
            i++;
        }
        if(ordre[i]==0){
            end+=i;
            return;
        }
        i++;
        if(ordre[i]==13&&ordre[i]!=10)i++;
        
        
        x="";
        while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
            x.push_back(ordre[i]);            
            i++;
        }
        if(ordre[i]==0){
            end+=i;
            return;
        }
        i++;
        if(ordre[i]==13&&ordre[i]!=10)i++;
        if(x=="BUY")bs=0;
        else if(x=="SELL")bs=1;
        

        stock="";
        while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
            stock.push_back(ordre[i]);            
            i++;
        }
        if(ordre[i]==0){
            end+=i;
            return;
        }
        i++;
        if(ordre[i]==13&&ordre[i]!=10)i++;
        

        if(ordre[i]=='$'){
            i++;
            x="";
            while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
                x.push_back(ordre[i]);            
                i++;
            }
            if(ordre[i]==0){
                end+=i;
                return;
            }
            i++;
            if(ordre[i]==13&&ordre[i]!=10)i++;
            
            price = stoi(x);
        }

        if(ordre[i]=='#'){
            i++;
            x="";
            while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
                x.push_back(ordre[i]);            
                i++;
            }
            if(ordre[i]==0){
                end+=i;
                return;
            }
            i++;
            if(ordre[i]==13&&ordre[i]!=10)i++;
            
            num = stoi(x);
        }
        
        x="";
        while(ordre[i]!=10&&ordre[i]!=32&&ordre[i]!=0&&ordre[i]!=13){
            x.push_back(ordre[i]);            
            i++;
        }
        if(ordre[i]==0){
            if(stoi(x)==-1){
                timeEXP=-1;
            }
            else{
                timeEXP=(timeIN+stoi(x)); 
            }
            end+=i;
            return;
        }
        i++;
        if(ordre[i]==13&&ordre[i]!=10)i++;
        
        if(stoi(x)==-1){
            timeEXP=-1;
        }
        else{
            timeEXP=(timeIN+stoi(x)); 
        }
        end+=i;
        return;       

    }

    bool operator < (const order& B) const {
			if (price < B.price) return true;
			else if (price > B.price) return false;
			if (timeIN < B.timeIN) return true;
			else if (timeIN > B.timeIN) return false;
			if (broker < B.broker) return true;
			else return false;
	}
    
	bool operator ==(const order& B) const {
		return id==B.id;
	}

	void display(){
        cerr<<endl;
        cerr<<"__________________________________"<<endl;
        cerr<<"Order id: "<<id<<endl;
        cerr<<"Broker: "<<broker<<endl;
        cerr<<"Stock: "<<stock<<endl;
        cerr<<"Price: "<<price<<endl;
        cerr<<"quantity: "<<num<<endl;
        cerr<<"TimeIN: "<<timeIN<<endl;
        cerr<<"TimeEXP: "<<timeEXP<<endl;
        cerr<<"bs: "<<bs<<endl;
        cerr<<endl;
        cerr<<"__________________________________"<<endl;
        cerr<<endl;
    }

};
int order::idc = 0;

void quote(order& y){
    //string x = "KarGoExpress SELL AMD $120 #32 1";
    string x="";
    x+=to_string(y.timeIN);
    x+=" ";
    x+="tejassharma_yashjonjale";
    if(y.bs==1){
        x+=" BUY";
    }
    else{
        x+=" SELL ";
    }
    x+=y.stock;
    x+=" $";
    x+=to_string(y.price);
    x+=" #";
    x+=to_string(y.num);
    x+=" ";
    x+=to_string(y.timeEXP-y.timeIN);
    x+="\n";
    std::fstream file("outputs/output"+to_string(y.thread)+".txt", std::ios::out);
    file << x;
    file.close();

}


class market{
    public:
    class stock{
        public:
        string name;
        int quant;
        dict<order, int> bookBuy;
		dict<int, dict<order, bool>> timeExpBuy;
		dict<order, int> bookSell;
		dict<int, dict<order, bool>> timeExpSell;
        stock(string nm,int qnt=0):name(nm),quant(qnt){}
        bool operator == (const stock& B) const {
			if (name!= B.name || quant != B.quant) return false;
			return true;
		}
		bool operator < (const stock& B) const {
			if(name<B.name)return true;
            if(name>B.name)return false;
            if(quant<B.quant)return true;
            return false;
		}


    };
    int timer;
    dict<stock,bool> stocks;
    Queue<order*> que1;
    Queue<order*> que2;
    int eod;
    int qi;
    int prevTEXP;
    int profit=0;
    market(){
        qi=1;
        timer=0;
        prevTEXP=0;
        eod=0;
    }
    void end(){
        eod=1;
        cout<<"Total Profits are: $"<<profit<<endl;
    }
    void begin(){
        while(true){
            if(eod==1)return;
            order* x;
            order** temp;
            if(qi==1){
                if(que2.isEmpty()){
                    qi=2;
                    quote_arbitrage();
                    continue;
                }
                que2.QDelete(temp);
                x=*temp;
            }
            else if(qi==2){
                if(que1.isEmpty()){
                    quote_arbitrage();
                    qi=1;
                    continue;
                }
                que1.QDelete(temp);
                x=*temp;
            }
            cerr<<"1 ksdikcj"<<endl;
            this->deleteEXP(this->timer);
            cerr<<"2 ksdikcj"<<endl;
            if(this->matchX(x)==0){
                cerr<<"3 jswnfj"<<endl;
                this->insert(x);
                cerr<<4<<" inwifn"<<endl;
            }
            cerr<<"repeat"<<endl;
        }
    }
    void deleteEXP(int time){
        for(int j = prevTEXP+1;j<=time;j++){
            for(auto itr1 = stocks.begin();!itr1.isNull();itr1++){
                stock& stk = itr1.key();
                dict<order, bool> out = stk.timeExpBuy[j];
                for (auto itr = out.begin(); !itr.isNull(); itr++) {
                    stk.bookBuy.remove(itr.key());
                }
                out = stk.timeExpSell[j];
                for (auto itr = out.begin(); !itr.isNull(); itr++) {
                    stk.bookSell.remove(itr.key());
                }
                stk.timeExpBuy.remove(j);
                stk.timeExpSell.remove(j);
            }
        }
		return;
    }
    
    int matchX(order* x){
        for(auto itr = stocks.begin();!itr.isNull();itr++){
            if(itr.key().name!=x->stock)continue;
            stock& stk = itr.key();
            if(x->bs==1){
                //buy
                int ToSell=x->num;
                auto itr2 = stk.bookBuy.end();
                for(;!itr2.isNull();itr2--){
                    if(x->thread!=itr2.key().thread){
                        continue;
                    }
                    if(ToSell<itr2.key().num){
                        itr2.key().num-=ToSell;
                        ToSell=0;
                        break;
                    }
                    int n = itr2.key().num;
                    int exp = itr2.key().timeEXP;
                    ToSell-=n;
                    stk.timeExpBuy[exp].remove(itr2.key());
                    stk.bookBuy.remove(itr2.key());
                    

                }
                if(ToSell==0)return 1;
                else{
                    x->num=ToSell;
                    return 0;
                }


                

            }
            else{
                //sell

                int ToBuy=x->num;
                auto itr2 = stk.bookSell.begin();
                for(;!itr2.isNull();itr2++){
                    if(x->thread!=itr2.key().thread){
                        continue;
                    }
                    if(ToBuy<itr2.key().num){
                        itr2.key().num-=ToBuy;
                        ToBuy=0;
                        break;
                    }
                    int n = itr2.key().num;
                    int exp = itr2.key().timeEXP;
                    ToBuy-=n;
                    stk.timeExpSell[exp].remove(itr2.key());
                    stk.bookSell.remove(itr2.key());
                    

                }
                if(ToBuy==0)return 1;
                else{
                    x->num=ToBuy;
                    return 0;
                }
            }

        }

    }
    void insert(order* x){
        for(auto itr=stocks.begin();!itr.isNull();itr++){   

            if(itr.key().name!=x->stock)continue;
			
			if (x->bs==0) {
				itr.key().bookBuy[*x] = x->timeEXP;
				itr.key().timeExpBuy[x->timeEXP][*x] = false;
			}
			else {
				itr.key().bookSell[*x] = x->timeEXP;
				itr.key().timeExpSell[x->timeEXP][*x] = false;
			}
            break;
        }
    }
    void quote_arbitrage(){
        auto top_itr = stocks.begin();
        for(;!top_itr.isNull();top_itr++){
            stock& stk = top_itr.key();
            auto buybookitr = stk.bookBuy.end();
            auto sellbookitr = stk.bookSell.begin();
            int nbuy=0;
            int nsell=0;
            while(buybookitr.key().price>=(sellbookitr.key().price)&&!buybookitr.isNull()&&!sellbookitr.isNull()){
                
                buybookitr.key().num--;nbuy++;
                profit+=buybookitr.key().price;
                if(buybookitr.key().num==0){
                    order ord1 = buybookitr.key();
                    ord1.num=nbuy;
                    ord1.timeIN=timer;
                    quote(ord1);
                    //int n = itr2.key().num;
                    int exp = buybookitr.key().timeEXP;
                    //ToSell-=n;
                    stk.timeExpBuy[exp].remove(buybookitr.key());
                    stk.bookBuy.remove(buybookitr.key());
                    nbuy=0;
                    buybookitr--;
                }
                sellbookitr.key().num--;nsell++;
                profit-=sellbookitr.key().price;
                if(sellbookitr.key().num==0){
                    order ord2 = sellbookitr.key();
                    ord2.num=nsell;
                    ord2.timeIN=timer;
                    quote(ord2);
                    int exp = sellbookitr.key().timeEXP;
                    //ToSell-=n;
                    stk.timeExpSell[exp].remove(sellbookitr.key());
                    stk.bookSell.remove(sellbookitr.key());
                    nsell=0;
                    sellbookitr++;
                }



            }
            if(nbuy!=0){
                order ord1 = buybookitr.key();
                ord1.num=nbuy;
                ord1.timeIN=timer;
                quote(ord1);
            }
            if(nsell!=0){
                order ord2 = sellbookitr.key();
                ord2.timeIN=timer;
                ord2.num=nsell;
                quote(ord2);
            }

        }


    }
};

struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    int thread;
    market* mkt;
    //trader* killer;
    ClientInfo(int socket, struct sockaddr_in& address,int i, market* m) : socket(socket), address(address), thread(i),mkt(m) {}
    ClientInfo() {};
};

// Function to handle a client connection
void *handleClient(void *arg) {
    ClientInfo *clientInfo = static_cast<ClientInfo *>(arg);
    char buffer[BUFFER_SIZE];

    std::cout << "Connected to client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
    market* mkt = clientInfo->mkt;
    bool fl=1;


    while (true) {

        // Receive data from the client
        //sleep(0.5);
        ssize_t bytesRead = recv(clientInfo->socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Error or connection closed
            if (bytesRead == 0) {
                std::cout << "Connection closed by client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
            } else {
                perror("Recv error");
            }
            break;
        } else {
            // Print the received message
            //std::lock_guard<std::mutex> guard(::printMutex);
            buffer[bytesRead] = '\0';
            //vector<order*> y;
            int i=0;
            while(true){
                if(((buffer+i)==NULL)||buffer[i]==0||buffer[i]=='$'){
                    cerr<<"something weird"<<endl;
                    if(buffer[i]==0){
                        cerr<<"0 spotted"<<endl;
                    }
                    if(buffer[i]=='$'){
                        cerr<<"dollar spotted"<<endl;
                        fl=0;
                    }
                    break;
                }
                order* x = new order(buffer+i,i,clientInfo->thread);
                mkt->timer=x->timeIN;
                //x->display();
                //cerr<<endl<<"display over"<<endl;
                if(mkt->qi==1){
                    mkt->que1.QInsert(x);
                    //cerr<<mkt->qi<<" "<<" insertion over"<<mkt->que1.size()<<endl;
                }
                else{
                    mkt->que2.QInsert(x);
                    //cerr<<mkt->qi<<" "<<" insertion over"<<mkt->que2.size()<<endl;

                }
                
                //x->display();
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            //std::cout <<bytesRead<<" "<< buffer << std::endl;
        }
        if(fl==0){
            
            break;
        
        }


    }

    // Close the client socket
    close(clientInfo->socket);
    delete clientInfo;
    pthread_exit(NULL);
}


int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8886);  // Port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {  // Maximum 5 pending connections
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    std::cout << "Trader is listening on port 8888..." << std::endl;
    
    market mkt;
    std::vector<pthread_t> clientThreads;

    for(int i = 0; i < NUM_THREADS; i++) {
        // Accept incoming connections
        int clientSocket;
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1) {
            perror("Accept error");
            continue;  // Continue listening for other connections
        }

        // Create a thread to handle this client
        ClientInfo *clientInfo = new ClientInfo(clientSocket, clientAddr,i+1,&mkt);
		std::cout << i<<" Connected to client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
		//std::cout << "clientinfo"<<clientInfo->socket<<endl;
        pthread_t clientThread;
        //create trader;
        


        if (pthread_create(&clientThread, NULL, handleClient, clientInfo) != 0) {
            perror("Thread creation error");
            delete clientInfo;
            continue;  // Continue listening for other connections
        }

        // Store the thread ID for later joining
        clientThreads.push_back(clientThread);
    }
    cerr<<"market begins"<<endl;
    mkt.begin();
    // Join all client threads (clean up)
    for (auto &thread : clientThreads) {
        pthread_join(thread, NULL);
    }
    mkt.end();
    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}
