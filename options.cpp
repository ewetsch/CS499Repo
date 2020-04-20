#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>


// ----------------Testing database connection and query---------------------

// ok make sure you include the header
#include <mysql/mysql.h>



//Definitions for accessing MySQL database for reads
#define SQLADDRESS "localhost"
#define SQLUSERNAME "*******"
#define SQLPASSWORD "********"
#define SQLDB "ovs"
#define SQLPORT 3306
void database_test(std::vector<float> &bCall, std::vector<float> &bPut, std::vector<float> &aCall, std::vector<float> &aPut, std::vector<float> &sPrices, float &currPrice, std::string ticker, std::string &currDate, std::string tarDate, float &traDays, float &volatility){

MYSQL* conn;
conn = mysql_init(0);
conn = mysql_real_connect(conn, SQLADDRESS, SQLUSERNAME, SQLPASSWORD, SQLDB, SQLPORT, NULL, 0);

//MYSQL optData;
std::string sqlQuery;
std::string curDate_test = "2020-04-17";
int volType = -1;

const int MAXLEN = 80;
char cDate[MAXLEN];
time_t t = time(0);
strftime(cDate, MAXLEN, "%Y-%m-%d", localtime(&t));
currDate = cDate;

    std::vector<int> eqIds; //List of eqIds-simple one level vector
    MYSQL_RES* qRes;
    MYSQL_ROW qRow;
    if(volType < 0) { //Implied

        //Other queries dont't work if the markets haven't closed on the current trading day.
        sqlQuery = "select date_ from ovscalendar where date_<'"+currDate+"' and calType='2'";
        mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
        qRes = mysql_store_result(conn);
        while(qRow=mysql_fetch_row(qRes)) {
            currDate = qRow[0];
      }
      //THE EXP DATE MUST BE A FRIDAY. THIS IS WHEN ALL OF THEM EXPIRE
        sqlQuery = "SELECT op.date_, eqm.ticker, oc.putCall, oc.strike, oc.expDate, op.bid, op.ask, op.volume, op.openInt, oc.corpAction FROM optprice AS op LEFT JOIN optcontract AS oc ON oc.optId=op.optId AND op.date_ between oc.startDate AND oc.endDate LEFT JOIN eqmaster AS eqm ON eqm.eqid=oc.eqId AND op.date_ between eqm.startDate and eqm.endDate WHERE eqm.ticker='"+ticker+"' AND op.date_ ='"+currDate+"' AND oc.expDate='"+tarDate+"' ORDER BY oc.opraRoot, oc.putCall, oc.strike";
        mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
        qRes = mysql_store_result(conn);
        int size = 0;
        while(qRow=mysql_fetch_row(qRes)) {
            
            eqIds.push_back(atoi(qRow[0]));
            //std::cout << size << " " << qRow[0] << " " << qRow[1] << " " << qRow[2] << " " << qRow[3] << " " << qRow[4] << " " << qRow[5] << " " << qRow[6] << " " << qRow[7] << " " << qRow[8] << std::endl;
            //std::cout << (MYSQL_ROW)(eqIds[0]) << std::endl;
            size++;
            std::string pc = qRow[2];
          //  std::cout << pc << std::endl;
            if(pc == "P"){
                //std::cout << pc << std::endl;
                sPrices.push_back(atof(qRow[3]));
                aPut.push_back(atof(qRow[6]));
                bPut.push_back(atof(qRow[5]));
            }
            else if(pc == "C"){
                //std::cout << pc << std::endl;
                aCall.push_back(atof(qRow[6]));
                bCall.push_back(atof(qRow[5]));
            }
        }
        
        sqlQuery = "SELECT eqp.eqid,eqm.ticker,eqm.issuer,eqp.date_,eqp.close_, iv.ivMid FROM eqprice AS eqp LEFT JOIN eqmaster AS eqm ON eqm.eqid=eqp.eqid AND eqp.date_ between eqm.startDate AND eqm.endDate LEFT JOIN ivcmpr as iv on eqp.eqId = iv.eqId and eqp.date_=iv.date_ and iv.strike=100 WHERE eqm.ticker = '"+ticker+"' and eqp.date_='"+currDate+"'    LIMIT 1";
        mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
        qRes = mysql_store_result(conn);
        while(qRow=mysql_fetch_row(qRes)) {
            volatility = atof(qRow[5]);
            currPrice = atof(qRow[4]);
        }

        sqlQuery = "SELECT count(date_) AS days FROM ovscalendar WHERE date_ between \""+currDate+"\" AND \""+tarDate+"\" AND calType = 2";      
        mysql_real_query(conn, sqlQuery.c_str(), sqlQuery.size());
        qRes = mysql_store_result(conn);
        while(qRow=mysql_fetch_row(qRes)) {
            traDays = atof(qRow[0]);
            //std::cout << qRow[0] << " " << traDays << "\n";
      }
    }
}

//------------ End test function --------------------------------------



//float vol, float currPrice, float days, int length, double *stanDevs, float *xAxis, float *yOpt
void optionPricesGraph(std::vector<float> &price, std::vector<float> &cumProb, float currPrice, float *xAxis, int length, float *yOpt, std::vector<float> &strikePrice, std::vector<float> &bidC, std::vector<float> &bidP, std::vector<float> &askC, std::vector<float> &askP){
    int sL = strikePrice.size()-1;
    float xAx,x1,x2,y1,y2;
    float zero = 0.0;
    
    for(int i = 0; i < sL; i++){
        price.push_back((strikePrice[i]+strikePrice[i+1])/2);
    }
    price.push_back(0.0);
    
    float midC[sL+1] = {};
    float midP[sL+1] = {};

    for(int i = 0; i < sL+1; i++){
        midC[i] = (bidC[i] + askC[i])/2;
        midP[i] = (bidP[i] + askP[i])/2;
        
    }

    float debitP[sL] = {};
    float debitC[sL] = {};

/////////////??????????????HOW CAN THERE BE A NEGATIVE DEBIT??????????????????///////////////////////////////
    for(int i = 0; i < sL; i++){
        debitC[i] = midC[i] - midC[i+1];
        debitP[i] = midP[i+1] - midP[i];
    }
    

    float strikeDif = strikePrice[1] - strikePrice[0];
    

    float pBM[sL] = {};
    for(int i = 0; i < sL; i++){
        if(bidP[i] == 0){
            pBM[i] = 0;
        }
        else{
            pBM[i] = debitP[i]/strikeDif;
        }
    }
    
    float pAM[sL] = {};
    for(int i = 0; i < sL; i++){
        if(bidC[i] == 0){
            pAM[i] = 0;
        }
        else{
            pAM[i] = debitC[i]/strikeDif;
        }
    }

    for(int i = 0; i < sL; i++){
        if(price[i] < currPrice){
            cumProb.push_back(pBM[i]);
        }
        else{
            cumProb.push_back(1-pAM[i]);
        }
    }

    float occurence[sL+1] = {};
    occurence[0] = 0 - cumProb[0];
    for(int i = 1; i < sL; i++){
        occurence[i] = cumProb[i] - cumProb[i-1];
    }
    occurence[sL] = 0.0;

    

    int tr[length] = {};
    for(int i = 0; i < length; i++){
        int min = price[0];
        
        for(int j = 1; j < sL; j++){
            if(price[j] <= xAxis[i]){
                min = j;
                
            }
        }
        //std::cout << min << std::endl;
        tr[i] = min;
        xAx = xAxis[i];
        x1 = price[tr[i]];
        x2 = price[tr[i]+1];
        y1 = occurence[tr[i]];
        y2 = occurence[tr[i]+1];

        //std::cout << xAx << "    " << x1 << "    " << "    " << x2 << "    " << y1 << "    " << y2  << std::endl;
        
        yOpt[i] = std::max(zero,((x2-xAx)/(x2-x1)*y1)+((x1-xAx)/(x1-x2)*y2))*2;
        std::cout << yOpt[i]  << std::endl;
        
    }
}

void optionPriceProb(std::vector<float> &price, std::vector<float> &cumProb, float tarPrice){
    int min = 0;
    for(int i = 0; i < 21; i++){
        if(price[i] <= tarPrice){
            min = i;
        }
    }
    float x1,x2,y1,y2;
    x1 = price[min];
    x2 = price[min+1];
    y1 = cumProb[min];
    y2 = cumProb[min+1];
    float probBelow = (((x2-tarPrice)/(x2-x1))*y1) + (((x1-tarPrice)/(x1-x2))*y2);
    float probAbove = 1.0 - probBelow;
    std::cout << "Probability Below Target," << probBelow << "\nProbability Above Target," << probAbove << std::endl;
}


int main(int argc, char** argv){
    float periodsInaYear = 252;//This needs to be a float for later calculations

    /*
    //Command-Line Arguments:
    //argv[0] --> Executable Name (not important)
    //argv[1] --> Back Door Debugger Tool (0, or 1)
    //argv[2] --> Ticker Symbol (e.g. "JPM")
    //argv[3] --> Target Price (decimal format e.g. "135")
    //argv[4] --> Target Date (date format e.g. "3/20/2020")
    //
    //
    //Example of how to compile via command-line
    //  g++ -std=c++11 -o ./public/calculations/o options.cpp -lmysqlclient -L/usr/include/mysql
    //
    //Example of how to run program via command-line:
    //      ./test 1 JPM 100 2020-04-24
    */

    if(argc < 5){
        std::cout << "Error: Invalid Arguments" << std::endl;
        return 0;
    }


    bool debugger;
    float volatility,currPrice,tarPrice;
    std::string ticker, currDate, tarDate;
    float traDays;//This needs to be a float for later calculations

    std::string::size_type sz;

    debugger = std::atoi(argv[1]);
    ticker = argv[2];
    tarPrice = std::atof(argv[3]);
    tarDate = argv[4];


    std::vector<float> strikePrice;
    std::vector<float> bidCall;
    std::vector<float> askCall; 
    std::vector<float> askPut;
    std::vector<float> bidPut;

    std::vector<float> price;
    std::vector<float> cumProb;
    

    database_test(bidCall, bidPut, askCall, askPut, strikePrice, currPrice,ticker,currDate,tarDate,traDays,volatility);



    int length = 161;

    //Standard Deviation Array
    double stanDevs[length] = {};
    //X-Axis values for graphing
    float xAxis[length] = {};
    //Line values for LogNormal graphing
    float yNorm[length] = {};
    //Line values for Option Prices graphing
    float yOpt[length] = {};


    //Fill Standard Deviation Array
    double sd = -4;
    for(int i = 0; i < length; i++){
        stanDevs[i] = sd;
        sd += 0.05;
    }

    //Fill X-Axis array

    /*
    //rootDYxV is a helper variable for xAxis calculations:
    //we use it for every xAxis calculation so we don't have to recalculate this value 160+ times
    //rootDYxV is the (square root of (trading days divided by periods in a year)) times volatility
    */
    float rootDYxV = sqrt(traDays/periodsInaYear) * volatility;
    //std::cout << traDays << "    " << volatility << "    " << currPrice << "      " << currDate << std::endl;
    for(int i = 0; i < length; i++){
        xAxis[i] = (exp (rootDYxV*stanDevs[i])) * currPrice;
        //std::cout << xAxis[i] << std::endl;
    }

    

    optionPricesGraph(price, cumProb, currPrice, xAxis, length, yOpt, strikePrice, bidCall, bidPut, askCall, askPut);

    /*Placeholder Output Format*/
    /*vvvvvvvvvvvvvvvvvvvvvvvvv*/
    
    std::cout << "$PL1T" << std::endl;
    std::cout << "PROBABILITY ANALYSIS" << std::endl;
	std::cout << "Ticker," << ticker << 
				"\nCurrent Price," << currPrice <<
                "\nTarget Price," << tarPrice << 
				"\nVolatility," << volatility << 
				"\nCurrent Date," << currDate << 
				"\nTarget Date," << tarDate << std::endl;
    /*^^^^^^^^^^^^^^^^^^^^^^^^^*/
    /*Placeholder Output Format*/

    optionPriceProb(price, cumProb, tarPrice);
    
    /*
    for(int i = 0; i < bidCall.size(); i++){
        std::cout << strikePrice[i] << " " << bidCall[i] << " " << askCall[i] << " " << bidPut[i] << " " << askPut[i] << std::endl;
    }

    std::cout << currPrice << " " << currDate << std::endl;
    */
    return 0;
}